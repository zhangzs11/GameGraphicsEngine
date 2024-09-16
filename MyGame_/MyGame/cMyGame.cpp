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


	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
	{
		m_gameObject.GetRigidBodyState().velocity = Math::sVector(-1.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		m_gameObject.GetRigidBodyState().velocity = Math::sVector(1.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		m_gameObject.GetRigidBodyState().velocity = Math::sVector(0.0f, 1.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		m_gameObject.GetRigidBodyState().velocity = Math::sVector(0.0f, -1.0f, 0.0f);
	}
	else
	{
		m_gameObject.GetRigidBodyState().velocity = Math::sVector(0.0f, 0.0f, 0.0f);
	}

}
void eae6320::cMyGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	// m_gameObject.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_gameObject.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	float color[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
	color[0] = (sinf(i_elapsedSecondCount_systemTime) + 1.0f) / 2.0f;
	eae6320::Graphics::SubmitBackgroundColor(color);

	if (m_gameObject.GetMesh() && m_gameObject.GetEffect())
	{
		eae6320::Graphics::SubmitMatrixLocalToWorld(m_gameObject.GetRigidBodyState().PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
		eae6320::Graphics::SubmitMeshEffectPair(m_gameObject.GetMesh(), m_gameObject.GetEffect());
	}
	
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Logging::OutputMessage((std::string("Initialize Window : ") + GetMainWindowName()).c_str());

	// Initialize mesh
	// ---------------

	eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[] = {
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.5f, 0.5f, 0.0f }
	};
	uint16_t indexData[] = { 0, 2, 1 };

	auto result = eae6320::Graphics::cMesh::CreateMesh(m_mesh, vertexData, static_cast<uint16_t>(std::size(vertexData)),
		indexData, static_cast<uint16_t>(std::size(indexData)));
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	// Initialize effect
	// -----------------

	uint8_t renderStateBits = 0;
	eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
	eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
	eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
	eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

	result = eae6320::Graphics::cEffect::CreateEffect(m_effect, "data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/loopGradient.shader",
		renderStateBits);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize effect");
		return result;
	}

	// Initialize GameObject
	// ---------------------

	m_gameObject.SetMesh(m_mesh);
	m_gameObject.SetEffect(m_effect);

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

	if (m_effect)
	{
		m_effect->DecrementReferenceCount();
		m_effect = nullptr;
	}

	return Results::Success;
}
