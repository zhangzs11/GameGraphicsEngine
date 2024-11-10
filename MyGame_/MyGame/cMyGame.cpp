// Includes
//=========


#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Graphics/sLight.h>
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
		m_gameObject_house.GetRigidBodyState().acceleration += Math::sVector(-1.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		m_gameObject_house.GetRigidBodyState().acceleration += Math::sVector(1.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		m_gameObject_house.GetRigidBodyState().acceleration += Math::sVector(0.0f, 1.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		m_gameObject_house.GetRigidBodyState().acceleration += Math::sVector(0.0f, -1.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed('N'))
	{
		m_gameObject_house.GetRigidBodyState().acceleration += Math::sVector(0.0f, 0.0f, 1.0f);
	}
	else if (UserInput::IsKeyPressed('M'))
	{
		m_gameObject_house.GetRigidBodyState().acceleration += Math::sVector(0.0f, 0.0f, -1.0f);
	}
	else
	{
		m_gameObject_house.GetRigidBodyState().acceleration = Math::sVector(0.0f, 0.0f, 0.0f);
		m_gameObject_house.GetRigidBodyState().velocity = Math::sVector(0.0f, 0.0f, 0.0f);
	}

	auto& cameraRigidBody = m_camera.GetRigidBodyState();
	const float movementSpeed = 50.0f;  // Units per second
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
	if (UserInput::IsKeyPressed('I'))
	{
		// m_gameObject_gear.GetRigidBodyState().angularSpeed = 1.1f;
	}
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
}
void eae6320::cMyGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{

}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	m_gameObject_house.Update(i_elapsedSecondCount_sinceLastUpdate);
	m_camera.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cMyGame::SubmitGameObjectToGraphics(cGameObject& i_gameObject, const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	if (i_gameObject.GetMesh() && i_gameObject.GetEffect())
	{
		eae6320::Graphics::SubmitMatrixLocalToWorld(i_gameObject.GetRigidBodyState().PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
		eae6320::Graphics::SubmitMaterial(i_gameObject.GetMaterial());
		eae6320::Graphics::SubmitMeshEffectPair(i_gameObject.GetMesh(), i_gameObject.GetEffect());
	}
}

void eae6320::cMyGame::SubmitCameraToGraphics(cCamera& i_camera, const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	auto worldToCameraTransform = i_camera.GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
	auto cameraToProjectedTransform = i_camera.GetCameraToProjectedTransform();
	eae6320::Graphics::SubmitCameraData(worldToCameraTransform, cameraToProjectedTransform, i_camera.GetPosition());
}

void eae6320::cMyGame::SubmitLightDataToGraphics(eae6320::Graphics::sDirectionalLight& i_dirL,
	                                             eae6320::Graphics::sPointLight& i_pointL,
	                                             eae6320::Graphics::sSpotLight& i_spotL)
{
	eae6320::Graphics::SubmitLightData(i_dirL, i_pointL, i_spotL);
}

void eae6320::cMyGame::SubmitShadowDataToGraphics(eae6320::Graphics::ShadowEffect* i_Shadoweffect,
	eae6320::Graphics::sDirectionalLight& i_dirL)
{
	Math::cMatrix_transformation WorldToLightCameraTransform = Math::cMatrix_transformation::CreateWorldToCameraTransform(
		Math::cQuaternion::LookAt(i_dirL.direction, Math::sVector(1, 0, 0)), 
		i_dirL.position);

	Math::cMatrix_transformation CameraToProjectedTransform_orthographic = Math::cMatrix_transformation::CreateCameraToProjectedTransform_orthographic(
		100.0f, -100.0f, 100.0f, -100.0f, 0.01f, 200.0f
	);

	Math::cMatrix_transformation T = Math::cMatrix_transformation(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	eae6320::Graphics::SubmitShadowData(i_Shadoweffect, 
		WorldToLightCameraTransform,
		CameraToProjectedTransform_orthographic,
		T * CameraToProjectedTransform_orthographic * WorldToLightCameraTransform);
}

void eae6320::cMyGame::SubmitSkyboxDataToGraphics(eae6320::Graphics::SkyboxEffect* i_skyboxeffect,
	eae6320::Graphics::cMesh* i_cubemesh)
{
	eae6320::Graphics::SubmitSkyboxData(i_skyboxeffect, i_cubemesh);
}

void eae6320::cMyGame::SubmitPostProcessingDataToGraphics(eae6320::Graphics::PostProcessingEffect* i_postProcessingeffect)
{
	eae6320::Graphics::SubmitPostProcessingData(i_postProcessingeffect);
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	eae6320::Graphics::SubmitBackgroundColor(color);
	
	SubmitGameObjectToGraphics(m_gameObject_house, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
	SubmitGameObjectToGraphics(m_gameObject_plane, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);
	// SubmitGameObjectToGraphics(m_gameObject_skybox, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);

	SubmitLightDataToGraphics(m_directionalLight, m_pointLight, m_spotLight);
	SubmitCameraToGraphics(m_camera, i_elapsedSecondCount_systemTime, i_elapsedSecondCount_sinceLastSimulationUpdate);

	SubmitShadowDataToGraphics(m_effect_shadowMap,
		                       m_directionalLight);

	SubmitSkyboxDataToGraphics(m_effect_skybox, m_mesh_cube);

	SubmitPostProcessingDataToGraphics(m_effect_postProcessing);
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Logging::OutputMessage((std::string("Initialize Window : ") + GetMainWindowName()).c_str());

	// Initialize mesh
	// ---------------

	/*auto result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_plane, "data/Meshes/plane_gl.binmesh");
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

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_helix, "data/Meshes/helix_complex.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}*/

	auto result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_cube, "data/Meshes/cube.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_plane, "data/Meshes/plane_gl.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_house, "data/Meshes/house_debug.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	/*result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_Alien, "data/Meshes/Alien.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}*/

	// Initialize effect
	// -----------------

	uint8_t renderStateBits = 0;
	eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthTesting(renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthWriting(renderStateBits);
	eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

	result = eae6320::Graphics::LightingEffect::CreateLightingEffect(
		                                              m_effect_light, 
		                                              "data/Shaders/Vertex/light_VS.binshader", 
		                                              "data/Shaders/Fragment/light_PS.binshader",
		                                              renderStateBits, 
		                                              "data/Textures/house.bintexture", 
		                                              "data/Textures/grass.bintexture",
		                                              eae6320::Graphics::eSamplerType::Linear,
		                                              eae6320::Graphics::eSamplerType::Comparison_less_equal);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize ligth_1 effect");
		return result;
	}


	result = eae6320::Graphics::LightingEffect::CreateLightingEffect(
		                                              m_effect_light2,
		                                              "data/Shaders/Vertex/light_VS.binshader",
		                                              "data/Shaders/Fragment/light_PS.binshader",
		                                              renderStateBits, 
		                                              "data/Textures/grass.bintexture",
		                                              "data/Textures/grass.bintexture",
		                                              eae6320::Graphics::eSamplerType::Linear,
		                                              eae6320::Graphics::eSamplerType::Comparison_less_equal);

	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize light_2 effect");
		return result;
	}

	// SKYBOX EFFECT
	uint8_t skybox_renderStateBits = 0;
	eae6320::Graphics::RenderStates::DisableAlphaTransparency(skybox_renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthTesting(skybox_renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthWriting(skybox_renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthTestLessEqual(skybox_renderStateBits);
	eae6320::Graphics::RenderStates::EnableDrawingBothTriangleSides(skybox_renderStateBits);

	result = eae6320::Graphics::SkyboxEffect::CreateSkyboxEffect(
		m_effect_skybox,
		"data/Shaders/Vertex/skybox_VS.binshader",
		"data/Shaders/Fragment/skybox_PS.binshader",
		skybox_renderStateBits, 
		"data/Textures/desertcube1024.bintexture", 
		eae6320::Graphics::eSamplerType::Linear);

	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize effect");
		return result;
	}

	// SHAODW MAP EFFECT
	uint8_t shadowRenderStateBits = 0;
	eae6320::Graphics::RenderStates::DisableAlphaTransparency(shadowRenderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthTesting(shadowRenderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthWriting(shadowRenderStateBits);
	eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(shadowRenderStateBits);


	result = eae6320::Graphics::ShadowEffect::CreateShadowEffect(
		                                              m_effect_shadowMap,
		                                              "data/Shaders/Vertex/shadow_VS.binshader",
		                                              shadowRenderStateBits);

	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize shadow map effect");
		return result;
	}

	// POSTPROCESSING EFFECT
	uint8_t postprocessingRenderStateBits = 0;
	eae6320::Graphics::RenderStates::DisableAlphaTransparency(postprocessingRenderStateBits);
	eae6320::Graphics::RenderStates::DisableDepthTesting(postprocessingRenderStateBits);
	eae6320::Graphics::RenderStates::DisableDepthWriting(postprocessingRenderStateBits);
	eae6320::Graphics::RenderStates::EnableDrawingBothTriangleSides(postprocessingRenderStateBits);

	result = eae6320::Graphics::PostProcessingEffect::CreatePostProcessingEffect(
		m_effect_postProcessing,
		"data/Shaders/Vertex/fullScreenTriangle.binshader",
		"data/Shaders/Fragment/postProcess_PS.binshader",
		shadowRenderStateBits,
		eae6320::Graphics::eSamplerType::Linear);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize shadow map effect");
		return result;
	}

	// Initialize GameObject
	// ---------------------
	eae6320::Graphics::sMaterial mat(eae6320::Math::sVector4(1.0f, 1.0f, 1.0f, 1.0f),          // ambient
		                             eae6320::Math::sVector4(1.64f, 1.64f, 1.64f, 1.0f),          // diffuse
		                             eae6320::Math::sVector4(0.5f, 0.5f, 0.5f, 1.0f),          // specular
		                             eae6320::Math::sVector4(0.5f, 0.5f, 0.5f, 1.0f));         // reflect

	m_gameObject_skybox.SetMesh(m_mesh_cube);
	//m_gameObject_skybox.SetEffect(m_effect_skybox);
	m_gameObject_skybox.SetPosition(eae6320::Math::sVector(0.0f, 0.0f, 0.0f));
	
	m_gameObject_house.SetMesh(m_mesh_house);
	m_gameObject_house.SetEffect(m_effect_light);
	m_gameObject_house.SetPosition(eae6320::Math::sVector(0.0f, -10.0f, 0.0f));
	m_gameObject_house.SetMaxVelocity(2.0f);
	m_gameObject_house.SetMaterial(mat);

	m_gameObject_plane.SetMesh(m_mesh_plane);
	m_gameObject_plane.SetEffect(m_effect_light2);
	m_gameObject_plane.SetPosition(eae6320::Math::sVector(0.0f, -12.0f, 0.0f));
	m_gameObject_plane.SetMaterial(mat);

	m_directionalLight = eae6320::Graphics::sDirectionalLight(eae6320::Math::sVector4(0.5f, 0.5f, 0.5f, 1.0f), //ambient
		                                                      eae6320::Math::sVector4(0.6f, 0.6f, 0.6f, 1.0f), //diffuse
		                                                      eae6320::Math::sVector4(0.0f, 0.0f, 0.0f, 1.0f), //specular
		                                                      eae6320::Math::sVector(-0.4f, -0.3f, 0.0f),      //direction
		                                                      eae6320::Math::sVector(50.0f, 100.5f, 0.0f));     //position

	m_pointLight = eae6320::Graphics::sPointLight(eae6320::Math::sVector4(0.0f, 0.0f, 0.0f, 1.0f),             //ambient
		                                          eae6320::Math::sVector4(0.0f, 1500.0f, 150.0f, 1.0f),             //diffuse
		                                          eae6320::Math::sVector4(0.0f, 1.0f, 0.0f, 1.0f),             //specular
		                                          eae6320::Math::sVector(50.0f, -10.0f, 0.0f),                    //position
		                                          10000.0f,                                                    //range
		                                          eae6320::Math::sVector(0.1f, 1.0f, 0.8f));                   //att

	m_spotLight = eae6320::Graphics::sSpotLight(eae6320::Math::sVector4(0.0f, 0.0f, 0.0f, 1.0f),               //ambient
		                                        eae6320::Math::sVector4(50.0f, 0.0f, 50.0f, 1.0f),               //diffuse
		                                        eae6320::Math::sVector4(0.0f, 0.0f, 0.0f, 1.0f),               //specular
		                                        eae6320::Math::sVector(0.0f, 5.0f, 0.0f),                      //position
		                                        10000.0f,                                                         //range
		                                        eae6320::Math::sVector(0.0f, -1.0f, 0.0f),                    //direction
		                                        10.0f,                                                          //spot
		                                        eae6320::Math::sVector(1.0f, 1.0f, 1.0f));                     //att
	



	// Initialize Camera
	// -----------------

	m_camera.SetProjectionParameters(eae6320::Math::ConvertDegreesToRadians(45.0f), 1.0f, 20.0f, 200.0f);
	m_camera.SetPosition(eae6320::Math::sVector(0.0f, 0.0f, 2.0f));
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
	if (m_mesh_cube)
	{
		m_mesh_cube->DecrementReferenceCount();
		m_mesh_cube = nullptr;
	}
	if (m_mesh_house)
	{
		m_mesh_house->DecrementReferenceCount();
		m_mesh_house = nullptr;
	}
	if (m_effect_light)
	{
		m_effect_light->DecrementReferenceCount();
		m_effect_light = nullptr;
	}
	if (m_effect_light2)
	{
		m_effect_light2->DecrementReferenceCount();
		m_effect_light2 = nullptr;
	}
	// SKYBOX
	if (m_effect_skybox)
	{
		m_effect_skybox->DecrementReferenceCount();
		m_effect_skybox = nullptr;
	}
	// SHADOW MAP
	if (m_effect_shadowMap)
	{
		m_effect_shadowMap->DecrementReferenceCount();
		m_effect_shadowMap = nullptr;

	}
	// POST PROCESSING
	if (m_effect_postProcessing)
	{
		m_effect_postProcessing->DecrementReferenceCount();
		m_effect_postProcessing = nullptr;

	}
	return Results::Success;
}
