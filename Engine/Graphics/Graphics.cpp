// Includes
//=========

#include "Graphics.h"

// #include "Includes.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cRenderState.h"
#include "cShader.h"
#include "cVertexFormat.h"
#include "sContext.h"
#include "VertexFormats.h"
// #include "cMesh.h"
// #include "cEffect.h"
#include "cView.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <new>
#include <utility>

// Static Data
//============

namespace
{
	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame);

	// Submission Data
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;

		float backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		static constexpr size_t MAX_SUBMITTED_PAIRS = 100;
		size_t submittedPairCount = 0;  // Number of currently submitted pairs
		std::pair<eae6320::Graphics::cMesh*, eae6320::Graphics::cEffect*> meshEffectPairs[MAX_SUBMITTED_PAIRS];
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be in the process of being populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated and in the process of being rendered from in the render thread
	// (In other words, one is being produced while the other is being consumed)
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;

	// Geometry Data
	//--------------

	// eae6320::Graphics::cMesh s_mesh;
	// eae6320::Graphics::cMesh s_mesh2;

	// Shading Data
	//-------------

	// eae6320::Graphics::cEffect s_effect;
	// eae6320::Graphics::cEffect s_effect2;

	// View
	//-------------
	eae6320::Graphics::cView s_view;
}

// Helper Declarations
//====================

namespace
{
	// eae6320::cResult InitializeGeometry();
	// eae6320::cResult InitializeShadingData();
	eae6320::cResult InitializeViews(const eae6320::Graphics::sInitializationParameters& i_initializationParameters);
}

// Interface
//==========

// Submission
//-----------

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;
	constantData_frame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_frame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

// Render
//-------

void eae6320::Graphics::SubmitMeshEffectPair(eae6320::Graphics::cMesh* i_mesh, eae6320::Graphics::cEffect* i_effect)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	auto& currentFrameData = *s_dataBeingSubmittedByApplicationThread;
	if (currentFrameData.submittedPairCount < sDataRequiredToRenderAFrame::MAX_SUBMITTED_PAIRS)
	{
		// Store the mesh-effect pair
		currentFrameData.meshEffectPairs[currentFrameData.submittedPairCount] = std::make_pair(i_mesh, i_effect);
		++currentFrameData.submittedPairCount;

		// Increment the reference count for both the mesh and the effect
		i_mesh->IncrementReferenceCount();
		i_effect->IncrementReferenceCount();
	}
	else
	{
		// Handle the case where the maximum number of pairs is exceeded
		eae6320::Logging::OutputError("The maximum number of submitted mesh-effect pairs has been exceeded!");
	}
}

void eae6320::Graphics::SubmitBackgroundColor(const float i_backgroundColor[4])
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	auto& currentFrameData = *s_dataBeingSubmittedByApplicationThread;

	// Copy the background color
	for (size_t i = 0; i < 4; ++i)
	{
		currentFrameData.backgroundColor[i] = i_backgroundColor[i];
	}
}

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		if (Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread))
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
			// Once the pointers have been swapped the application loop can submit new data
			if (!s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal())
			{
				EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
				Logging::OutputError("Failed to signal that new render data can be submitted");
				UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited");
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
			Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
			UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);
	auto* const dataRequiredToRenderFrame = s_dataBeingRenderedByRenderThread;

	// Clear render buffer with the submitted background color
	s_view.Clear(dataRequiredToRenderFrame->backgroundColor);

	// Update the frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_frame = dataRequiredToRenderFrame->constantData_frame;
		s_constantBuffer_frame.Update(&constantData_frame);
	}

	// Draw submitted mesh-effect pairs
	for (size_t i = 0; i < dataRequiredToRenderFrame->submittedPairCount; ++i)
	{
		auto& pair = dataRequiredToRenderFrame->meshEffectPairs[i];
		pair.second->Bind();  // Bind effect
		pair.first->Draw();   // Draw mesh
	}

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (or "swapped" with the "front buffer", which is the image that is actually being displayed)
	{
		auto result = sContext::g_context.Present();
	}

	// After all of the data that was submitted for this frame has been used
	// you must make sure that it is all cleaned up and cleared out
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		// (At this point in the class there isn't anything that needs to be cleaned up)
		//dataRequiredToRenderFrame	// TODO
		for (size_t i = 0; i < dataRequiredToRenderFrame->submittedPairCount; ++i)
		{
			auto& pair = dataRequiredToRenderFrame->meshEffectPairs[i];
			pair.first->DecrementReferenceCount();
			pair.second->DecrementReferenceCount();

			pair.first = nullptr;
			pair.second = nullptr;
		}
		dataRequiredToRenderFrame->submittedPairCount = 0;
	}
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without context");
		return result;
	}
	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_frame.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_frame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without frame constant buffer");
			return result;
		}
	}
	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data has been submitted from the application thread");
			return result;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data can be submitted from the application thread");
			return result;
		}
	}
	// Initialize and Bind the views
	{
		if (!(result = InitializeViews(i_initializationParameters)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the views");
			return result;
		}
		s_view.Bind();
	}
	//// Initialize the shading data
	//{
	//	if (!(result = InitializeShadingData()))
	//	{
	//		EAE6320_ASSERTF(false, "Can't initialize Graphics without the shading data");
	//		return result;
	//	}
	//}
	//// Initialize the geometry
	//{
	//	if (!(result = InitializeGeometry()))
	//	{
	//		EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
	//		return result;
	//	}
	//}

	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	// Clean up the View
	if (!(result = s_view.CleanUp()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up the View");
	}
	//// Clean up the Effect
	//if (!(result = s_mesh.CleanUp()))
	//{
	//	EAE6320_ASSERTF(false, "Failed to clean up the mesh");
	//}
	//// Clean up the Mesh
	//if (!(result = s_effect.CleanUp()))
	//{
	//	EAE6320_ASSERTF(false, "Failed to clean up the effect");
	//}
	//// Clean up the Effect2
	//if (!(result = s_mesh2.CleanUp()))
	//{
	//	EAE6320_ASSERTF(false, "Failed to clean up the mesh");
	//}
	//// Clean up the Mesh2
	//if (!(result = s_effect2.CleanUp()))
	//{
	//	EAE6320_ASSERTF(false, "Failed to clean up the effect");
	//}
	
	// Clean up submitted mesh/effect pairs from both frames
	for (auto& frameData : s_dataRequiredToRenderAFrame)
	{
		for (size_t i = 0; i < frameData.submittedPairCount; ++i)
		{
			auto& pair = frameData.meshEffectPairs[i];
			if (pair.first)
			{
				pair.first->DecrementReferenceCount();
				pair.first = nullptr;
			}
			if (pair.second)
			{
				pair.second->DecrementReferenceCount();
				pair.second = nullptr;
			}
		}
		frameData.submittedPairCount = 0;  // Reset submitted pair count for future safety
	}

	// Clean up constant buffer
	{
		const auto result_constantBuffer_frame = s_constantBuffer_frame.CleanUp();
		if (!result_constantBuffer_frame)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_frame;
			}
		}
	}

	{
		const auto result_context = sContext::g_context.CleanUp();
		if (!result_context)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_context;
			}
		}
	}

	return result;
}

// Helper Definitions
//===================

namespace
{
	/*eae6320::cResult InitializeGeometry()
	{
		auto result = eae6320::Results::Success;

		eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[] = {
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
		};
		uint16_t indexData[] = { 0, 3, 2, 0, 2, 1 };
		if (!(result = s_mesh.Initialize(vertexData, static_cast<uint16_t>(std::size(vertexData)), indexData, static_cast<uint16_t>(std::size(indexData)))))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
			return result;
		}

		eae6320::Graphics::VertexFormats::sVertex_mesh vertexData2[] = {
			{ -1.0f, -1.0f, 0.0f },
			{ -0.5f, 0.0f, 0.0f },
			{ 0.0f, -1.0f, 0.0f },
		};
		uint16_t indexData2[] = { 0, 1, 2 };
		if (!(result = s_mesh2.Initialize(vertexData2, static_cast<uint16_t>(std::size(vertexData2)), indexData2, static_cast<uint16_t>(std::size(indexData2)))))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
			return result;
		}
		return result;
	}

	eae6320::cResult InitializeShadingData()
	{
		auto result = eae6320::Results::Success;

		uint8_t renderStateBits = 0;
		eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
		eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
		eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
		eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

		if (!(result = s_effect.Initialize("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/animatedColor.shader", renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the shading data");
			return result;
		}

		if (!(result = s_effect2.Initialize("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/standard.shader", renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the shading data");
			return result;
		}

		return result;
	}*/

	eae6320::cResult InitializeViews(const eae6320::Graphics::sInitializationParameters& i_initializationParameters)
	{
		auto result = eae6320::Results::Success;

		if (!(result = s_view.Initialize(i_initializationParameters)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the View data");
			return result;
		}

		return result;
	}
}
