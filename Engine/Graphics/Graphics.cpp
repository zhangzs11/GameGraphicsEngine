// Includes
//=========

#include "Graphics.h"

#include "Direct3D/Includes.h"
#include "sContext.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cRenderState.h"
#include "cShader.h"
#include "cVertexFormat.h"
#include "sContext.h"
#include "VertexFormats.h"
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
	eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame,
		eae6320::Graphics::eConstantBufferEffectType::Light);
	eae6320::Graphics::cConstantBuffer s_constantBuffer_drawCall(eae6320::Graphics::ConstantBufferTypes::DrawCall,
		eae6320::Graphics::eConstantBufferEffectType::Light);
	eae6320::Graphics::cConstantBuffer s_shadow_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame,
		eae6320::Graphics::eConstantBufferEffectType::Shadow);
	// Submission Data
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sLight_Frame constantData_frame;
		eae6320::Graphics::ConstantBufferFormats::sShadow_Frame shadow_constantData_frame;
		eae6320::Graphics::ShadowEffect* shadowEffect;
		eae6320::Graphics::SkyboxEffect* skyboxEffect;
		eae6320::Graphics::PostProcessingEffect* postProcessingEffect;

		float backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		static constexpr size_t MAX_SUBMITTED_PAIRS = 100;
		size_t submittedPairCount = 0;  // Number of currently submitted pairs
		std::pair<eae6320::Graphics::cMesh*, eae6320::Graphics::LightingEffect*> meshEffectPairs[MAX_SUBMITTED_PAIRS];

		eae6320::Graphics::cMesh* skyboxCube;

		eae6320::Graphics::ConstantBufferFormats::sDrawCall constantData_drawCall[MAX_SUBMITTED_PAIRS];
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

	// View
	//-------------
	eae6320::Graphics::cView_RTV s_Scene_RTV;
	eae6320::Graphics::cView_DSV s_Scene_DSV;
	eae6320::Graphics::cView_SRV s_Lit_SRV;
	eae6320::Graphics::cView_SRV s_SceneDepth_SRV; // input post process

	eae6320::Graphics::cView_DSV s_shadowMap_DSV;
	eae6320::Graphics::cView_SRV s_shadowMap_SRV;

	eae6320::Graphics::cView_RTV s_Skybox_RTV;
	eae6320::Graphics::cView_SRV s_Skybox_SRV; // input post process

	eae6320::Graphics::cView_RTV s_PostProcess_RTV;

}

// Helper Declarations
//====================

namespace
{
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

void eae6320::Graphics::SubmitCameraData(const Math::cMatrix_transformation& i_transform_worldToCamera, const Math::cMatrix_transformation& i_transform_cameraToProjected, const Math::sVector& i_eyePosW)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;

	// Set the camera transformation matrices
	constantData_frame.g_transform_worldToCamera = i_transform_worldToCamera;
	constantData_frame.g_transform_cameraToProjected = i_transform_cameraToProjected;
	constantData_frame.g_EyePosW = i_eyePosW;
}

void eae6320::Graphics::SubmitLightData(const eae6320::Graphics::sDirectionalLight& i_directionalLight,
	                 const eae6320::Graphics::sPointLight& i_pointLight,
	                 const eae6320::Graphics::sSpotLight& i_spotLight)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;
	constantData_frame.g_DirLight = i_directionalLight;
	constantData_frame.g_PointLight = i_pointLight;
	constantData_frame.g_SpotLight = i_spotLight;
}

void eae6320::Graphics::SubmitMeshEffectPair(eae6320::Graphics::cMesh* i_mesh, eae6320::Graphics::LightingEffect* i_effect)
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

void eae6320::Graphics::SubmitMatrixLocalToWorld(const eae6320::Math::cMatrix_transformation& i_transform_localToWorld)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	
	auto& currentFrameData = *s_dataBeingSubmittedByApplicationThread;
	if (currentFrameData.submittedPairCount < sDataRequiredToRenderAFrame::MAX_SUBMITTED_PAIRS)
	{
		currentFrameData.constantData_drawCall[currentFrameData.submittedPairCount].g_transform_localToWorld = i_transform_localToWorld;

		currentFrameData.constantData_drawCall[currentFrameData.submittedPairCount].g_transform_localToWorld_Inv_Transpose = i_transform_localToWorld.GetInverse().GetTranspose();
	}
	else
	{
		// Handle the case where the maximum number is exceeded
		eae6320::Logging::OutputError("The maximum number of submitted Matrix LocalToWorld has been exceeded!");
	}
}

void eae6320::Graphics::SubmitMaterial(const eae6320::Graphics::sMaterial& i_material)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	auto& currentFrameData = *s_dataBeingSubmittedByApplicationThread;
	if (currentFrameData.submittedPairCount < sDataRequiredToRenderAFrame::MAX_SUBMITTED_PAIRS)
	{
		currentFrameData.constantData_drawCall[currentFrameData.submittedPairCount].g_Material = i_material;
	}
	else
	{
		// Handle the case where the maximum number is exceeded
		eae6320::Logging::OutputError("The maximum number of submitted Matrix LocalToWorld has been exceeded!");
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

void eae6320::Graphics::SubmitShadowData(ShadowEffect* i_Shadoweffect,
	const eae6320::Math::cMatrix_transformation& i_transform_worldToLightCamera,
	const eae6320::Math::cMatrix_transformation& i_transform_LightcameraToProjected,
	const eae6320::Math::cMatrix_transformation& i_ShadowTransform)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	auto& currentFrameData = *s_dataBeingSubmittedByApplicationThread;
	currentFrameData.shadowEffect = i_Shadoweffect;
	currentFrameData.shadow_constantData_frame.g_transform_worldToCamera = i_transform_worldToLightCamera;
	currentFrameData.shadow_constantData_frame.g_transform_cameraToProjected = i_transform_LightcameraToProjected;
	currentFrameData.constantData_frame.g_ShadowTransform = i_ShadowTransform; // ShadowView * ShadowProj * T
}
void eae6320::Graphics::SubmitSkyboxData(eae6320::Graphics::SkyboxEffect* i_skyboxeffect,
	                                     eae6320::Graphics::cMesh* i_cubemesh)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	auto& currentFrameData = *s_dataBeingSubmittedByApplicationThread;
	currentFrameData.skyboxEffect = i_skyboxeffect;
	currentFrameData.skyboxCube = i_cubemesh;
}

void eae6320::Graphics::SubmitPostProcessingData(eae6320::Graphics::PostProcessingEffect* i_postprocessEffect)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);

	auto& currentFrameData = *s_dataBeingSubmittedByApplicationThread;
	currentFrameData.postProcessingEffect = i_postprocessEffect;
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

	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);
    // RenderShadow map
    // ----------------
	//
	direct3dImmediateContext->OMSetRenderTargets(0, nullptr, s_shadowMap_DSV.m_depthStencilView);
	s_shadowMap_DSV.Clear(nullptr);
	direct3dImmediateContext->RSSetViewports(1, s_shadowMap_DSV.m_viewPort);

	// Update the frame constant in Shadow Effect buffer
	{
		auto& shadow_constantData_frame = dataRequiredToRenderFrame->shadow_constantData_frame;
		s_shadow_constantBuffer_frame.Update(&shadow_constantData_frame);
		s_shadow_constantBuffer_frame.Bind(static_cast<uint_fast8_t>(eShaderType::Vertex));
		dataRequiredToRenderFrame->shadowEffect->Bind();
	}

	// Draw submitted mesh in pairs
	for (size_t i = 0; i < dataRequiredToRenderFrame->submittedPairCount; ++i)
	{
		auto& pair = dataRequiredToRenderFrame->meshEffectPairs[i];
		auto& constantData_drawCall = dataRequiredToRenderFrame->constantData_drawCall[i];
		s_constantBuffer_drawCall.Update(&constantData_drawCall);
		s_constantBuffer_drawCall.Bind(static_cast<uint_fast8_t>(eShaderType::Vertex));

		pair.first->Draw();
	}

	// Render Forward Object
	// --------------------------------
	// 
	// Update the frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_frame = dataRequiredToRenderFrame->constantData_frame;
		s_constantBuffer_frame.Update(&constantData_frame);
		s_constantBuffer_frame.Bind(
			static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
	}

	// Clear render buffer with the submitted background color
	constexpr unsigned int renderTargetCount = 1;
	direct3dImmediateContext->OMSetRenderTargets(renderTargetCount, 
		                                         &(s_Scene_RTV.m_renderTargetView),
		                                         s_Scene_DSV.m_depthStencilView);
	s_Scene_RTV.Clear(dataRequiredToRenderFrame->backgroundColor);
	s_Scene_DSV.Clear(nullptr);
	direct3dImmediateContext->RSSetViewports(1, s_Scene_RTV.m_viewPort);

	// Draw submitted mesh-effect pairs
	for (size_t i = 0; i < dataRequiredToRenderFrame->submittedPairCount; ++i)
	{
		auto& pair = dataRequiredToRenderFrame->meshEffectPairs[i];

		// Update and Rebind the draw call constant buffer
		{
			auto& constantData_drawCall = dataRequiredToRenderFrame->constantData_drawCall[i];
			s_constantBuffer_drawCall.Update(&constantData_drawCall);
			s_constantBuffer_drawCall.Bind(static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}

		pair.second->SetShadowMapSRV(&s_shadowMap_SRV);
		pair.second->Bind();  // Bind effect
		pair.first->Draw();   // Draw mesh
	}

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	direct3dImmediateContext->PSSetShaderResources(2, 1, nullSRV);

	// Render SKYBOX
	// --------------------------------
	// 
	// constexpr unsigned int renderTargetCount = 1;

	auto& skyboxEffect = dataRequiredToRenderFrame->skyboxEffect;


	direct3dImmediateContext->OMSetRenderTargets(1,
		&(s_Skybox_RTV.m_renderTargetView),
		nullptr);

	s_Skybox_RTV.Clear(dataRequiredToRenderFrame->backgroundColor);
	direct3dImmediateContext->RSSetViewports(1, s_Skybox_RTV.m_viewPort);

	skyboxEffect->SetDepthSRV(&s_SceneDepth_SRV);

	skyboxEffect->SetLitSRV(&s_Lit_SRV);

	skyboxEffect->Bind();

	auto& skyboxCube = dataRequiredToRenderFrame->skyboxCube;
	skyboxCube->Draw();


	// Post Processing Effect
	// --------------------------------
	// 
	auto& postProcessingEffect = dataRequiredToRenderFrame->postProcessingEffect;

	direct3dImmediateContext->OMSetRenderTargets(1,
		&(s_PostProcess_RTV.m_renderTargetView),
		nullptr);

	s_PostProcess_RTV.Clear(dataRequiredToRenderFrame->backgroundColor);
	direct3dImmediateContext->RSSetViewports(1, s_PostProcess_RTV.m_viewPort);

	postProcessingEffect->SetDepthSRV(&s_SceneDepth_SRV);
	postProcessingEffect->SetLitSRV(&s_Skybox_SRV);

	postProcessingEffect->Bind();
	// Draw VertexID : 0, 1, 2
	direct3dImmediateContext->Draw(3, 0);



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
	// TODO:
	// CLEAN EFFECT and other MEsh...
	// Like this LOOP
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

		if (!(result = s_constantBuffer_drawCall.Initialize()))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without draw call constant buffer");
			return result;
		}

		// Shadow CBuffer
		if (result = s_shadow_constantBuffer_frame.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			//s_constantBuffer_frame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
			//	static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
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
	}

	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	// Clean up the View
	if (!(result = s_Scene_RTV.CleanUp()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up the View");
	}
	if (!(result = s_Scene_DSV.CleanUp()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up the View");
	}
	if (!(result = s_Lit_SRV.CleanUp()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up the View");
	}
	if (!(result = s_SceneDepth_SRV.CleanUp()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up the View");
	}
	if (!(result = s_shadowMap_DSV.CleanUp()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up the View");
	}
	if (!(result = s_shadowMap_SRV.CleanUp()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up the View");
	}
	if (!(result = s_Skybox_RTV.CleanUp()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up the View");
	}
	if (!(result = s_Skybox_SRV.CleanUp()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up the View");
	}
	if (!(result = s_PostProcess_RTV.CleanUp()))
	{
		EAE6320_ASSERTF(false, "Failed to clean up the View");
	}
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

		const auto result_constantBuffer_drawCall = s_constantBuffer_drawCall.CleanUp();
		if (!result_constantBuffer_drawCall)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_drawCall;
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
	eae6320::cResult InitializeViews(const eae6320::Graphics::sInitializationParameters& i_initializationParameters)
	{
		auto result = eae6320::Results::Success;

		if (!(result = s_Scene_RTV.Initialize(i_initializationParameters,
			                                   eae6320::Graphics::eRenderTargetType::Texture)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the View data");
			return result;
		}

		if (!(result = s_Scene_DSV.Initialize(i_initializationParameters)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the View data");
			return result;
		}
		if (!(result = s_Lit_SRV.Initialize(i_initializationParameters, 
			s_Scene_RTV.m_TextureBuffer,
			eae6320::Graphics::BufferType::RenderTarget)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the View data");
			return result;
		}
		if (!(result = s_SceneDepth_SRV.Initialize(i_initializationParameters, 
			s_Scene_DSV.m_TextureBuffer,
			eae6320::Graphics::BufferType::Depth)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the View data");
			return result;
		}
		if (!(result = s_shadowMap_DSV.Initialize(i_initializationParameters)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the View data");
			return result;
		}

		if (!(result = s_shadowMap_SRV.Initialize(i_initializationParameters, 
			s_shadowMap_DSV.m_TextureBuffer,
			eae6320::Graphics::BufferType::Depth)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the View data");
			return result;
		}
		if (!(result = s_Skybox_RTV.Initialize(i_initializationParameters, 
			                                   eae6320::Graphics::eRenderTargetType::Texture)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the View data");
			return result;
		}
		if (!(result = s_Skybox_SRV.Initialize(i_initializationParameters, 
			                                   s_Skybox_RTV.m_TextureBuffer, 
			                                   eae6320::Graphics::BufferType::RenderTarget)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the View data");
			return result;
		}
		if (!(result = s_PostProcess_RTV.Initialize(i_initializationParameters,
			                                        eae6320::Graphics::eRenderTargetType::Screen)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the View data");
			return result;
		}
		return result;
	}
}