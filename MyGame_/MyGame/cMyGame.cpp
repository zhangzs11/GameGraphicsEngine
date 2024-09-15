// Includes
//=========


#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/VertexFormats.h>
// Inherited Implementation
//=========================

// Run
//----

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
	// Is the user pressing the SPACE key?
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
	{
		SetSimulationRate(5.0f);
	}
	else
	{                
		SetSimulationRate(1.0f);
	}

	// Is the user pressing the LEFT key?
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
	{
		// one of the meshes not draw
		ifLeftPressed = true;
	}
	else
	{
		ifLeftPressed = false;
	}
	// Is the user pressing the RIGHT key?
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		// one of the meshes get drawn with a different effect
		ifRightPressed = true;
	}
	else
	{
		ifRightPressed = false;
	}

}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	float color[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
	color[0] = (sinf(i_elapsedSecondCount_systemTime) + 1.0f) / 2.0f;
	eae6320::Graphics::SubmitBackgroundColor(color);

	if (!ifLeftPressed)
	{
		eae6320::Graphics::SubmitMeshEffectPair(m_mesh, m_effect);
	}

	if (!ifRightPressed)
	{
		eae6320::Graphics::SubmitMeshEffectPair(m_mesh2, m_effect);
	}
	else
	{
		eae6320::Graphics::SubmitMeshEffectPair(m_mesh2, m_effect2);
	}

	float timeFraction = fmod(i_elapsedSecondCount_systemTime, 1.0f);
	// make mesh effect changes happen automatically as time passes
	if (timeFraction < 0.5f)
	{
		eae6320::Graphics::SubmitMeshEffectPair(m_mesh3, m_effect2);
	}
	else
	{
		eae6320::Graphics::SubmitMeshEffectPair(m_mesh4, m_effect2);
	}
	
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Logging::OutputMessage((std::string("Initialize Window : ") + GetMainWindowName()).c_str());

	// Initialize mesh
	// ---------------
	
	// mesh1
	eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[] = {
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, -1.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -0.5f, 1.0f, 0.0f },
		{ -1.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ -0.5f, 0.0f, 0.0f },
	};
	uint16_t indexData[] = { 0, 2, 1 };

	auto result = eae6320::Graphics::cMesh::CreateMesh(m_mesh, vertexData, static_cast<uint16_t>(std::size(vertexData)),
		indexData, static_cast<uint16_t>(std::size(indexData)));
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	// mesh2
	uint16_t indexData2[] = { 0, 4, 3 };

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh2, vertexData, static_cast<uint16_t>(std::size(vertexData)),
		indexData2, static_cast<uint16_t>(std::size(indexData2)));
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	// mesh3
	uint16_t indexData3[] = { 0, 5, 6 };

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh3, vertexData, static_cast<uint16_t>(std::size(vertexData)),
		indexData3, static_cast<uint16_t>(std::size(indexData3)));
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	// mesh4
	uint16_t indexData4[] = { 7, 8, 9 };

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh4, vertexData, static_cast<uint16_t>(std::size(vertexData)),
		indexData4, static_cast<uint16_t>(std::size(indexData4)));
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	// Initialize effect
	// -----------------
    
	// effect1
	uint8_t renderStateBits = 0;
	eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
	eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
	eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
	eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

	result = eae6320::Graphics::cEffect::CreateEffect(m_effect, "data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/animatedColor.shader",
		renderStateBits);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize effect");
		return result;
	}

	// effect2
	result = eae6320::Graphics::cEffect::CreateEffect(m_effect2, "data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/loopGradient.shader",
		renderStateBits);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize effect");
		return result;
	}
	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	eae6320::Logging::OutputMessage((std::string("Clean Up Window : ") + GetMainWindowName()).c_str());

	if (m_mesh)
	{
		m_mesh->DecrementReferenceCount();
		m_mesh = nullptr;
	}

	if (m_mesh2)
	{
		m_mesh2->DecrementReferenceCount();
		m_mesh = nullptr;
	}

	if (m_effect)
	{
		m_effect->DecrementReferenceCount();
		m_effect = nullptr;
	}

	if (m_effect2)
	{
		m_effect2->DecrementReferenceCount();
		m_effect = nullptr;
	}

	return Results::Success;
}
