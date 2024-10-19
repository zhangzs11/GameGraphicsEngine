// Includes
//=========


#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Math/Functions.h>

#include <Engine/Time/Time.h>
#include <string>
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
		m_gameObject_pipe.GetRigidBodyState().acceleration = Math::sVector(-1.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		m_gameObject_pipe.GetRigidBodyState().acceleration = Math::sVector(1.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		m_gameObject_pipe.GetRigidBodyState().acceleration = Math::sVector(0.0f, 1.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		m_gameObject_pipe.GetRigidBodyState().acceleration = Math::sVector(0.0f, -1.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed('N'))
	{
		m_gameObject_pipe.GetRigidBodyState().acceleration = Math::sVector(0.0f, 0.0f, 1.0f);
	}
	else if (UserInput::IsKeyPressed('M'))
	{
		m_gameObject_pipe.GetRigidBodyState().acceleration = Math::sVector(0.0f, 0.0f, -1.0f);
	}
	else
	{
		m_gameObject_pipe.GetRigidBodyState().acceleration = Math::sVector(0.0f, 0.0f, 0.0f);
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

	//if (UserInput::IsKeyPressed('Q'))
	//{
	//	m_gameObject.SetMesh(m_mesh2);
	//}
	//if (UserInput::IsKeyPressed('E'))
	//{
	//	m_gameObject.SetMesh(m_mesh);
	//}

}
void eae6320::cMyGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	// m_gameObject.Update(i_elapsedSecondCount_sinceLastUpdate);
	// m_camera.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_gameObject_plane.Update(i_elapsedSecondCount_sinceLastUpdate);
	m_gameObject_gear.Update(i_elapsedSecondCount_sinceLastUpdate);
	m_gameObject_helix.Update(i_elapsedSecondCount_sinceLastUpdate);
	m_gameObject_pipe.Update(i_elapsedSecondCount_sinceLastUpdate);
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

	SubmitGameObjectToGraphics(m_gameObject_plane, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
	SubmitGameObjectToGraphics(m_gameObject_gear, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
	SubmitGameObjectToGraphics(m_gameObject_helix, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
	SubmitGameObjectToGraphics(m_gameObject_pipe, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);

	SubmitCameraToGraphics(m_camera, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Logging::OutputMessage((std::string("Initialize Window : ") + GetMainWindowName()).c_str());

	// Initialize mesh
	// ---------------

	auto result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_plane, "data/Meshes/plane_gl.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_gear, "data/Meshes/torus_gl.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_helix, "data/Meshes/helix_gl.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	// result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_pipe, "data/Meshes/pipe_gl.binmesh");

	/*if (result = eae6320::Time::Initialize(); !result)
	{
		return result;
	}
	const uint64_t startTicks = eae6320::Time::GetCurrentSystemTimeTickCount();*/

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_pipe, "data/Meshes/Alien.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}
	// const uint64_t endTicks = eae6320::Time::GetCurrentSystemTimeTickCount();
	// const uint64_t elapsedTicks = endTicks - startTicks;
	// const double elapsedSeconds = eae6320::Time::ConvertTicksToSeconds(elapsedTicks);
	// eae6320::Logging::OutputMessage(("Bin file load and extract time: " + std::to_string(elapsedSeconds) + " seconds\n").c_str());

	// Initialize effect
	// -----------------

	uint8_t renderStateBits = 0;
	eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
	// eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
	// eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthTesting(renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthWriting(renderStateBits);
	eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

	result = eae6320::Graphics::cEffect::CreateEffect(m_effect_color, "data/Shaders/Vertex/standard.binshader", "data/Shaders/Fragment/standard.binshader",
		renderStateBits);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize effect");
		return result;
	}

	result = eae6320::Graphics::cEffect::CreateEffect(m_effect_animited_color, "data/Shaders/Vertex/standard.binshader", "data/Shaders/Fragment/animatedColor.binshader",
		renderStateBits);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize effect");
		return result;
	}

	// Initialize GameObject
	// ---------------------

	m_gameObject_plane.SetMesh(m_mesh_plane);
	m_gameObject_plane.SetEffect(m_effect_color);
	m_gameObject_plane.SetMaxVelocity(2.0f);
	m_gameObject_plane.SetPosition(eae6320::Math::sVector(0.0f, -2.0f, 0.0f));

	m_gameObject_gear.SetMesh(m_mesh_gear);
	m_gameObject_gear.SetEffect(m_effect_color);
	m_gameObject_gear.SetMaxVelocity(2.0f);
	m_gameObject_gear.SetPosition(eae6320::Math::sVector(0.0f, -1.0f, 2.0f));

	m_gameObject_helix.SetMesh(m_mesh_helix);
	m_gameObject_helix.SetEffect(m_effect_color);
	m_gameObject_helix.SetMaxVelocity(2.0f);
	m_gameObject_helix.SetPosition(eae6320::Math::sVector(2.0f, 1.0f, 0.0f));

	m_gameObject_pipe.SetMesh(m_mesh_pipe);
	m_gameObject_pipe.SetEffect(m_effect_color);
	m_gameObject_pipe.SetMaxVelocity(2.0f);
	m_gameObject_pipe.SetPosition(eae6320::Math::sVector(-2.0f, -1.0f, 0.0f));


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

	if (m_mesh_plane)
	{
		m_mesh_plane->DecrementReferenceCount();
		m_mesh_plane = nullptr;
	}

	if (m_mesh_gear)
	{
		m_mesh_gear->DecrementReferenceCount();
		m_mesh_gear = nullptr;
	}

	if (m_mesh_helix)
	{
		m_mesh_helix->DecrementReferenceCount();
		m_mesh_helix = nullptr;
	}

	if (m_mesh_pipe)
	{
		m_mesh_pipe->DecrementReferenceCount();
		m_mesh_pipe = nullptr;
	}

	if (m_effect_color)
	{
		m_effect_color->DecrementReferenceCount();
		m_effect_color = nullptr;
	}

	if (m_effect_animited_color)
	{
		m_effect_animited_color->DecrementReferenceCount();
		m_effect_animited_color = nullptr;
	}

	return Results::Success;
}
