// Includes
//=========


#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Math/Functions.h>

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
		m_gameObject.GetRigidBodyState().acceleration = Math::sVector(-1.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		m_gameObject.GetRigidBodyState().acceleration = Math::sVector(1.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		m_gameObject.GetRigidBodyState().acceleration = Math::sVector(0.0f, 1.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		m_gameObject.GetRigidBodyState().acceleration = Math::sVector(0.0f, -1.0f, 0.0f);
	}
	else
	{
		m_gameObject.GetRigidBodyState().acceleration = Math::sVector(0.0f, 0.0f, 0.0f);
	}

	auto& cameraRigidBody = m_camera.GetRigidBodyState();
	const float movementSpeed = 5.0f;  // Units per second
	constexpr float rotationSpeed = eae6320::Math::ConvertDegreesToRadians(60.0f);  // Radians per second

	// Movement: WASD for forward, backward, left, right
	if (UserInput::IsKeyPressed('W'))
	{
		cameraRigidBody.velocity = m_camera.GetForwardDirection() * movementSpeed;
	}
	else if (UserInput::IsKeyPressed('S'))
	{
		cameraRigidBody.velocity = m_camera.GetForwardDirection() * -movementSpeed;
	}
	else if (UserInput::IsKeyPressed('A'))
	{
		cameraRigidBody.velocity = m_camera.GetRightDirection() * -movementSpeed;
	}
	else if (UserInput::IsKeyPressed('D'))
	{
		cameraRigidBody.velocity = m_camera.GetRightDirection() * movementSpeed;
	}
	else
	{
		cameraRigidBody.velocity = Math::sVector(0.0f, 0.0f, 0.0f);
	}

	// Rotation: Arrow keys for pitch and yaw
	//if (UserInput::IsKeyPressed('I'))
	//{
	//	cameraRigidBody.angularVelocity_axis_local = m_camera.GetRightDirection();
	//	cameraRigidBody.angularSpeed = rotationSpeed;
	//}
	//else if (UserInput::IsKeyPressed('K'))
	//{
	//	cameraRigidBody.angularVelocity_axis_local = m_camera.GetRightDirection();
	//	cameraRigidBody.angularSpeed = -rotationSpeed;
	//}
	if (UserInput::IsKeyPressed('J'))
	{
		cameraRigidBody.angularVelocity_axis_local = Math::sVector(0.0f, 1.0f, 0.0f);  // Yaw rotation axis
		cameraRigidBody.angularSpeed = rotationSpeed;
	}
	else if (UserInput::IsKeyPressed('K'))
	{
		cameraRigidBody.angularVelocity_axis_local = Math::sVector(0.0f, 1.0f, 0.0f);  // Yaw rotation axis
		cameraRigidBody.angularSpeed = -rotationSpeed;
	}
	else
	{
		cameraRigidBody.angularSpeed = 0.0f;
	}

	if (UserInput::IsKeyPressed('Q'))
	{
		m_gameObject.SetMesh(m_mesh2);
	}
	if (UserInput::IsKeyPressed('E'))
	{
		m_gameObject.SetMesh(m_mesh);
	}

}
void eae6320::cMyGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	// m_gameObject.Update(i_elapsedSecondCount_sinceLastUpdate);
	// m_camera.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_gameObject.Update(i_elapsedSecondCount_sinceLastUpdate);
	m_camera.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cMyGame::SubmitGameObjectToGraphics(cGameObject& i_gameObject, const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	if (i_gameObject.GetMesh() && i_gameObject.GetEffect())
	{
		eae6320::Graphics::SubmitMatrixLocalToWorld(i_gameObject.GetRigidBodyState().PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
		eae6320::Graphics::SubmitMeshEffectPair(i_gameObject.GetMesh(), i_gameObject.GetEffect());
	}
}

void eae6320::cMyGame::SubmitCameraToGraphics(cCamera& i_camera, const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	auto worldToCameraTransform = i_camera.GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
	auto cameraToProjectedTransform = i_camera.GetCameraToProjectedTransform();
	eae6320::Graphics::SubmitCameraData(worldToCameraTransform, cameraToProjectedTransform);
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	float color[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
	color[0] = (sinf(i_elapsedSecondCount_systemTime) + 1.0f) / 2.0f;
	eae6320::Graphics::SubmitBackgroundColor(color);

	SubmitGameObjectToGraphics(m_gameObject, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);

	SubmitCameraToGraphics(m_camera, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Logging::OutputMessage((std::string("Initialize Window : ") + GetMainWindowName()).c_str());

	// Initialize mesh
	// ---------------

	auto result = eae6320::Graphics::cMesh::CreateMesh(m_mesh, "data/Meshes/Helix_onlyChangeIndex.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	// Initialize effect
	// -----------------

	uint8_t renderStateBits = 0;
	eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
	// eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
	// eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthTesting(renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthWriting(renderStateBits);
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
	m_gameObject.SetMaxVelocity(2.0f);

	// Initialize Camera
	// -----------------

	m_camera.SetProjectionParameters(eae6320::Math::ConvertDegreesToRadians(45.0f), 1.0f, 0.1f, 100000.0f);
	m_camera.SetPosition(eae6320::Math::sVector(0.0f, 0.0f, 10.0f));
	m_camera.SetOrientation(eae6320::Math::cQuaternion());

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
		m_mesh2 = nullptr;
	}

	if (m_effect)
	{
		m_effect->DecrementReferenceCount();
		m_effect = nullptr;
	}

	return Results::Success;
}
