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
#include <cmath>
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
		m_gameObject_tree.GetRigidBodyState().acceleration += Math::sVector(-1.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
	{
		m_gameObject_tree.GetRigidBodyState().acceleration += Math::sVector(1.0f, 0.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
	{
		m_gameObject_tree.GetRigidBodyState().acceleration += Math::sVector(0.0f, 1.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
	{
		m_gameObject_tree.GetRigidBodyState().acceleration += Math::sVector(0.0f, -1.0f, 0.0f);
	}
	else if (UserInput::IsKeyPressed('N'))
	{
		m_gameObject_tree.GetRigidBodyState().acceleration += Math::sVector(0.0f, 0.0f, 1.0f);
	}
	else if (UserInput::IsKeyPressed('M'))
	{
		m_gameObject_tree.GetRigidBodyState().acceleration += Math::sVector(0.0f, 0.0f, -1.0f);
	}
	else
	{
		m_gameObject_tree.GetRigidBodyState().acceleration = Math::sVector(0.0f, 0.0f, 0.0f);
		m_gameObject_tree.GetRigidBodyState().velocity = Math::sVector(0.0f, 0.0f, 0.0f);
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

	if (UserInput::IsKeyPressed('1'))
	{
		m_current_postprocess_effect = m_effect_postProcessing_Default;
	}

	if (UserInput::IsKeyPressed('2'))
	{
		m_current_postprocess_effect = m_effect_postProcessing_Distortion;
	}

	if (UserInput::IsKeyPressed('4'))
	{
		m_FXAA_QualitySubPix = 0.00f;
		m_FXAA_QualityEdgeThreshold = 0.333f;
		m_FXAA_QualityEdgeThresholdMin = 0.0833f;
	}

	if (UserInput::IsKeyPressed('5'))
	{
		m_FXAA_QualitySubPix = 0.50f;
		m_FXAA_QualityEdgeThreshold = 0.166f;
		m_FXAA_QualityEdgeThresholdMin = 0.0625f;
	}

	if (UserInput::IsKeyPressed('6'))
	{
		m_FXAA_QualitySubPix = 1.00f;
		m_FXAA_QualityEdgeThreshold = 0.063f;
		m_FXAA_QualityEdgeThresholdMin = 0.0312f;
	}

	// Rotation: Arrow keys for pitch and yaw
	if (UserInput::IsKeyPressed('I'))
	{
		// m_gameObject_table.GetRigidBodyState().angularSpeed = 0.1f;
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
	m_gameObject_tree.Update(i_elapsedSecondCount_sinceLastUpdate);
	m_gameObject_tree2.Update(i_elapsedSecondCount_sinceLastUpdate);
	// m_gameObject_marblebust.Update(i_elapsedSecondCount_sinceLastUpdate);
	m_camera.Update(i_elapsedSecondCount_sinceLastUpdate);
	m_lightCamera.Update(i_elapsedSecondCount_sinceLastUpdate);
	m_cascadedShadowManager.UpdateFrame(m_camera, m_lightCamera, testBox, i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cMyGame::SubmitGameObjectToGraphics(cGameObject& i_gameObject, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	if (i_gameObject.GetMesh() && i_gameObject.GetEffect())
	{
		eae6320::Graphics::SubmitMatrixLocalToWorld(i_gameObject.GetRigidBodyState().PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate));
		eae6320::Graphics::SubmitMaterial(i_gameObject.GetMaterial());

		for (size_t cascadeIndex = 0; cascadeIndex < m_cascadedShadowManager.m_CascadeLevels; ++cascadeIndex)
		{
			eae6320::Graphics::SubmitMatrixLightSpaceLocalToProjected(cascadeIndex,
				// m_cascadedShadowManager.m_ShadowProj[cascadeIndex] *
				// m_lightCamera.GetCameraToProjectedTransform() * 
				// m_cascadedShadowManager.m_ShadowProj[cascadeIndex] *
				// m_camera.GetCameraToProjectedTransform() * 
				(i_gameObject.GetRigidBodyState().PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate)),
				m_lightCamera.GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastSimulationUpdate),
				m_cascadedShadowManager.m_ShadowProj[cascadeIndex]
				/*Math::cMatrix_transformation::CreateCameraToProjectedTransform_orthographic(
					-100.0f, 100.0f, -100.0f, 100.0f, -5.0f, -100.0f
				)*/
				// m_camera.GetCameraToProjectedTransform()
			);

			/*eae6320::Graphics::SubmitMatrixLightSpaceLocalToProjected(cascadeIndex,
				Math::cMatrix_transformation::CreateCameraToProjectedTransform_orthographic(
					100.0f, -100.0f, 100.0f, -100.0f, 0.01f, 200.0f
				)
			*
				m_lightCamera.GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastSimulationUpdate)
			*
				(i_gameObject.GetRigidBodyState().PredictFutureTransform(i_elapsedSecondCount_sinceLastSimulationUpdate)));*/
		}

		eae6320::Graphics::SubmitMeshEffectPair(i_gameObject.GetMesh(), i_gameObject.GetEffect());
	}
}

void eae6320::cMyGame::SubmitCameraToGraphics(Graphics::cCamera& i_camera, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	auto worldToCameraTransform = i_camera.GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastSimulationUpdate);
	auto cameraToProjectedTransform = i_camera.GetCameraToProjectedTransform();
	eae6320::Graphics::SubmitCameraData(worldToCameraTransform, cameraToProjectedTransform, i_camera.GetPosition());
}

void eae6320::cMyGame::SubmitLightDataToGraphics(std::vector<eae6320::Graphics::sDirectionalLight>& i_directionalLights,
	                                             std::vector<eae6320::Graphics::sPointLight>& i_pointLights,
	                                             std::vector<eae6320::Graphics::sSpotLight>& i_spotLights,
	                                             const float i_elapsedSecondCount_systemTime)
{
	i_spotLights[0].position = (sinf(i_elapsedSecondCount_systemTime)) * eae6320::Math::sVector(10.0f, 0.0f, 0.0f)
		                + eae6320::Math::sVector(35.0f, 10.0f, 35.0f);

	i_pointLights[0].diffuse = eae6320::Math::sVector4(0.0f, 
		                                               10.0f * abs(sinf(i_elapsedSecondCount_systemTime)), 
		                                               200.0f * abs(cosf(i_elapsedSecondCount_systemTime)), 
		                                               1.0f);

	eae6320::Graphics::SubmitLightData(i_directionalLights, i_pointLights, i_spotLights);
}

void eae6320::cMyGame::SubmitShadowDataToGraphics(
	eae6320::Graphics::ShadowEffect* i_Shadoweffect,
	const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	Math::cMatrix_transformation T = Math::cMatrix_transformation(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	Math::sVector4 scales[4];
	Math::sVector4 offsets[4];

	for (size_t cascadeIndex = 0; cascadeIndex < m_cascadedShadowManager.m_CascadeLevels; ++cascadeIndex)
	{
		Math::cMatrix_transformation ShadowTexture = T * m_cascadedShadowManager.m_ShadowProj[cascadeIndex];

		scales[cascadeIndex] = Math::sVector4(
			ShadowTexture.m_00,  
			ShadowTexture.m_11,  
			ShadowTexture.m_22,  
			1.0f);               

		offsets[cascadeIndex] = Math::sVector4(
			ShadowTexture.m_03,  
			ShadowTexture.m_13,  
			ShadowTexture.m_23,  
			1.0f);               
	}

	float cascadeFrustumsEyeSpaceDepths[4] = { 10.0f, 30.0f, 50.0f, 100.0f};

	eae6320::Graphics::SubmitShadowData(
		i_Shadoweffect,
		m_lightCamera.GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastSimulationUpdate),
		offsets,
		scales,
		cascadeFrustumsEyeSpaceDepths,
		1,         // Visualize cascades with different colors (1 to enable, 0 to disable)
		-2,        // PCF kernel loop start value (-2 for a 5x5 PCF kernel)
		3,         // PCF kernel loop end value (3 for a 5x5 PCF kernel)
		0.01f,     // Minimum border padding (e.g., (kernelSize / 2) / shadowMapSize)
		0.99f,     // Maximum border padding (e.g., 1.0f - (kernelSize / 2) / shadowMapSize)
		0.005f,    // Shadow bias to reduce shadow artifacts
		10.0f,     // Blend area between cascades
		1.0f / 1024.0f // Texel size of the shadow map
	);
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

void eae6320::cMyGame::SubmitFXAADataToGraphics(eae6320::Graphics::PostProcessingEffect* i_FXAAffect,
	float g_TexelSize_x, float g_TexelSize_y,
	float g_QualitySubPix,
	float g_QualityEdgeThreshold,
	float g_QualityEdgeThresholdMin)
{
	eae6320::Graphics::SubmitFXAAData(
		i_FXAAffect,
		g_TexelSize_x, g_TexelSize_y,
		g_QualitySubPix,
		g_QualityEdgeThreshold,
		g_QualityEdgeThresholdMin);
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	eae6320::Graphics::SubmitBackgroundColor(color);
	
	for (int i = 0; i < 60; i++) {
		SubmitGameObjectToGraphics(m_gameObject_plane[i], i_elapsedSecondCount_sinceLastSimulationUpdate);
	}

	SubmitGameObjectToGraphics(m_gameObject_tree, i_elapsedSecondCount_sinceLastSimulationUpdate);
	SubmitGameObjectToGraphics(m_gameObject_tree2, i_elapsedSecondCount_sinceLastSimulationUpdate);
	SubmitGameObjectToGraphics(m_gameObject_marblebust_big, i_elapsedSecondCount_sinceLastSimulationUpdate);

	SubmitLightDataToGraphics(m_directionalLights, m_pointLights, m_spotLights,
		i_elapsedSecondCount_systemTime);
	SubmitCameraToGraphics(m_camera, i_elapsedSecondCount_sinceLastSimulationUpdate);

	SubmitShadowDataToGraphics(m_effect_shadowMap,
		i_elapsedSecondCount_sinceLastSimulationUpdate);

	SubmitSkyboxDataToGraphics(m_effect_skybox, m_mesh_cube);

	SubmitPostProcessingDataToGraphics(m_current_postprocess_effect);

	SubmitFXAADataToGraphics(m_effect_FXAA, 1.0f / 1024.0f, 1.0f / 1024.0f, 
		                     m_FXAA_QualitySubPix, 
		                     m_FXAA_QualityEdgeThreshold, 
		                     m_FXAA_QualityEdgeThresholdMin);
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::Logging::OutputMessage((std::string("Initialize Window : ") + GetMainWindowName()).c_str());

	// Initialize mesh
	// ---------------

	auto result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_cube, "data/Meshes/cube.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_plane, "data/Meshes/plane.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_tree, "data/Meshes/quiver_tree_all_change.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_table, "data/Meshes/table.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_marblebust, "data/Meshes/marble_bust.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_marblebust_big, "data/Meshes/garden_gnome.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	result = eae6320::Graphics::cMesh::CreateMesh(m_mesh_wall, "data/Meshes/wall.binmesh");
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize mesh");
		return result;
	}

	// Initialize effect
	// -----------------

	uint8_t renderStateBits = 0;
	eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthTesting(renderStateBits);
	eae6320::Graphics::RenderStates::EnableDepthWriting(renderStateBits);
	eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

	result = eae6320::Graphics::LightingEffect::CreateLightingEffect(
		                                              m_effect_tree, 
		                                              "data/Shaders/Vertex/light_VS.binshader", 
		                                              "data/Shaders/Fragment/light_NormalMap_CSM_PS.binshader",
		                                              renderStateBits, 
		                                              "data/Textures/quiver_tree_diffuse.bintexture", 
		                                              "data/Textures/quiver_tree_normal_dx.bintexture",
		                                              eae6320::Graphics::eSamplerType::Linear,
		                                              eae6320::Graphics::eSamplerType::Comparison_less_equal);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize ligth_1 effect");
		return result;
	}


	result = eae6320::Graphics::LightingEffect::CreateLightingEffect(
		                                              m_effect_plane,
		                                              "data/Shaders/Vertex/light_VS.binshader",
		                                              "data/Shaders/Fragment/light_NormalMap_CSM_PS.binshader",
		                                              renderStateBits, 
		                                              "data/Textures/ganges_diff_4k.bintexture",
		                                              "data/Textures/ganges_dx_nmap_4k.bintexture",
		                                              eae6320::Graphics::eSamplerType::Linear,
		                                              eae6320::Graphics::eSamplerType::Linear);

	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize light_2 effect");
		return result;
	}

	result = eae6320::Graphics::LightingEffect::CreateLightingEffect(
		m_effect_marblebust,
		"data/Shaders/Vertex/light_VS.binshader",
		"data/Shaders/Fragment/light_NormalMap_CSM_PS.binshader",
		renderStateBits,
		"data/Textures/garden_gnome_diff.bintexture",
		"data/Textures/garden_gnome_nor.bintexture",
		eae6320::Graphics::eSamplerType::Linear,
		eae6320::Graphics::eSamplerType::Linear);

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
		"data/Textures/grasscube1024.bintexture", 
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
		m_effect_postProcessing_Default,
		"data/Shaders/Vertex/fullScreenTriangle.binshader",
		"data/Shaders/Fragment/postProcess_PS.binshader",
		shadowRenderStateBits,
		eae6320::Graphics::eSamplerType::Linear);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize shadow map effect");
		return result;
	}

	m_current_postprocess_effect = m_effect_postProcessing_Default;

	result = eae6320::Graphics::PostProcessingEffect::CreatePostProcessingEffect(
		m_effect_postProcessing_Distortion,
		"data/Shaders/Vertex/fullScreenTriangle.binshader",
		"data/Shaders/Fragment/postProcess_Distortion_PS.binshader",
		shadowRenderStateBits,
		eae6320::Graphics::eSamplerType::Linear);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize shadow map effect");
		return result;
	}

	// FXAA EFFECT
	float m_FXAA_QualitySubPix = 1.00f;
	float m_FXAA_QualityEdgeThreshold = 0.166f;
	float m_FXAA_QualityEdgeThresholdMin = 0.0833f;

	result = eae6320::Graphics::PostProcessingEffect::CreatePostProcessingEffect(
		m_effect_FXAA,
		"data/Shaders/Vertex/fullScreenTriangle.binshader",
		"data/Shaders/Fragment/FXAA_post_PS.binshader",
		shadowRenderStateBits,
		eae6320::Graphics::eSamplerType::FXAA_LinearClamp);
	if (!result)
	{
		EAE6320_ASSERTF(false, "Failed to initialize shadow map effect");
		return result;
	}


	// Initialize GameObjects
	// ----------------------
	eae6320::Graphics::sMaterial mat(eae6320::Math::sVector4(0.8f, 0.8f, 0.8f, 1.0f),          // ambient
		                             eae6320::Math::sVector4(1.0f, 1.0f, 1.0f, 1.0f),          // diffuse
		                             eae6320::Math::sVector4(0.5f, 0.5f, 0.5f, 1.0f),          // specular
		                             eae6320::Math::sVector4(0.5f, 0.5f, 0.5f, 1.0f));         // reflect

	m_gameObject_skybox.SetMesh(m_mesh_cube);
	m_gameObject_skybox.SetPosition(eae6320::Math::sVector(0.0f, 0.0f, 0.0f));
	
	m_gameObject_tree.SetMesh(m_mesh_tree);
	m_gameObject_tree.SetEffect(m_effect_tree);
	m_gameObject_tree.SetPosition(eae6320::Math::sVector(35.0f, -15.0f, 35.0f));
	// m_gameObject_tree.SetMaxVelocity(2.0f);
	m_gameObject_tree.SetMaterial(mat);

	for (int i = 0; i < 60; i++) {
		m_gameObject_plane[i].SetMesh(m_mesh_plane);
		m_gameObject_plane[i].SetEffect(m_effect_plane);
		m_gameObject_plane[i].SetPosition(eae6320::Math::sVector(0.0f, -12.0f, 0.0f) + 
			                              eae6320::Math::sVector(20.0f * (i % 10), 0.0f, 20.0f * (i / 10)));
		m_gameObject_plane[i].SetMaterial(mat);
	}

	m_gameObject_marblebust_big.SetMesh(m_mesh_marblebust_big);
	m_gameObject_marblebust_big.SetEffect(m_effect_marblebust);
	m_gameObject_marblebust_big.SetPosition(eae6320::Math::sVector(80.0f, -15.0f, 65.0f));
	//m_gameObject_marblebust_big.SetMaxVelocity(2.0f);
	m_gameObject_marblebust_big.SetMaterial(mat);

	m_gameObject_tree2.SetMesh(m_mesh_tree);
	m_gameObject_tree2.SetEffect(m_effect_tree);
	m_gameObject_tree2.SetPosition(eae6320::Math::sVector(100.0f, -15.0f, 65.0f));
	// m_gameObject_tree2.SetMaxVelocity(2.0f);
	m_gameObject_tree2.GetRigidBodyState().angularSpeed = 0.5f;
	m_gameObject_tree2.SetMaterial(mat);


	// Initialize Lights
	// ---------------------
	eae6320::Graphics::sDirectionalLight directionalLight = eae6320::Graphics::sDirectionalLight(eae6320::Math::sVector4(0.3f, 0.3f, 0.3f, 1.0f), //ambient
		                                                      eae6320::Math::sVector4(0.7f, 0.7f, 0.7f, 1.0f),                                      //diffuse
		                                                      eae6320::Math::sVector4(1.0f, 1.0f, 1.0f, 1.0f),                                      //specular
		                                                      eae6320::Math::sVector(-0.6f, -1.0f, 0.0f),                                           //direction
		                                                      eae6320::Math::sVector(200.0f, 400.0f, 50.0f));                                         //position

	eae6320::Graphics::sPointLight pointLight = eae6320::Graphics::sPointLight(eae6320::Math::sVector4(0.0f, 0.1f, 0.0f, 1.0f),             //ambient
		                                          eae6320::Math::sVector4(0.0f, 100.0f, 0.0f, 1.0f),                                            //diffuse
		                                          eae6320::Math::sVector4(0.0f, 1.0f, 0.0f, 1.0f),                                            //specular
		                                          eae6320::Math::sVector(85.0f, 0.0f, 55.0f),                                                //position
		                                          10000.0f,                                                                                   //range
		                                          eae6320::Math::sVector(1.0f, 1.0f, 1.0f));                                                  //att

	eae6320::Graphics::sSpotLight spotLight = eae6320::Graphics::sSpotLight(eae6320::Math::sVector4(0.0f, 0.0f, 0.0f, 1.0f),               //ambient
		                                        eae6320::Math::sVector4(50.0f, 500.0f, 50.0f, 1.0f),                                         //diffuse
		                                        eae6320::Math::sVector4(0.0f, 0.0f, 0.0f, 1.0f),                                             //specular
		                                        eae6320::Math::sVector(45.0f, 10.0f, 35.0f),                                                 //position
		                                        30000.0f,                                                                                      //range
		                                        eae6320::Math::sVector(0.0f, -1.0f, 0.0f),                                                   //direction
		                                        50.0f,                                                                                       //spot
		                                        eae6320::Math::sVector(1.0f, 1.0f, 1.0f));                                                   //att
	
	m_directionalLights.push_back(directionalLight);
	m_pointLights.push_back(pointLight);
	m_spotLights.push_back(spotLight);


	// Initialize Camera
	// -----------------
	m_camera.SetType(eae6320::Graphics::eCameraType::Perspective);
	m_camera.SetProjectionParameters(eae6320::Math::ConvertDegreesToRadians(45.0f), 1280.0f/1024.0f, 1.0f, 200.0f);
	m_camera.SetPosition(eae6320::Math::sVector(100.0f, 0.0f, 100.0f));
	m_camera.SetOrientation(eae6320::Math::cQuaternion::LookAt(eae6320::Math::sVector(1.0f, 0.0f, 1.0f), 
		                                                      eae6320::Math::sVector(0.0f, 1.0f, 0.0f)));


	m_lightCamera.SetType(eae6320::Graphics::eCameraType::Perspective);
	m_lightCamera.SetProjectionParameters(eae6320::Math::ConvertDegreesToRadians(45.0f), 1280.0f / 1024.0f, 10.0f, 2000.0f);
	m_lightCamera.SetPosition(directionalLight.position);
	m_lightCamera.SetOrientation(eae6320::Math::cQuaternion::LookAt(-directionalLight.direction,
		eae6320::Math::sVector(0.0f, 0.0f, 1.0f)));

	testBox.Center = { 50.0f, 0.0f, 30.0f };
	testBox.Extents = { 150.0f, 150.0f, 150.0f };

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
	if (m_mesh_tree)
	{
		m_mesh_tree->DecrementReferenceCount();
		m_mesh_tree = nullptr;
	}
	if (m_mesh_table)
	{
		m_mesh_table->DecrementReferenceCount();
		m_mesh_table = nullptr;
	}
	if (m_mesh_marblebust)
	{
		m_mesh_marblebust->DecrementReferenceCount();
		m_mesh_marblebust = nullptr;
	}
	if (m_mesh_marblebust_big)
	{
		m_mesh_marblebust_big->DecrementReferenceCount();
		m_mesh_marblebust_big = nullptr;
	}
	if (m_mesh_wall)
	{
		m_mesh_wall->DecrementReferenceCount();
		m_mesh_wall = nullptr;
	}
	if (m_effect_tree)
	{
		m_effect_tree->DecrementReferenceCount();
		m_effect_tree = nullptr;
	}
	if (m_effect_plane)
	{
		m_effect_plane->DecrementReferenceCount();
		m_effect_plane = nullptr;
	}
	if (m_effect_marblebust)
	{
		m_effect_marblebust->DecrementReferenceCount();
		m_effect_marblebust = nullptr;
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
	if (m_effect_postProcessing_Default)
	{
		m_effect_postProcessing_Default->DecrementReferenceCount();
		m_effect_postProcessing_Default = nullptr;

	}
	if (m_effect_postProcessing_Distortion)
	{
		m_effect_postProcessing_Distortion->DecrementReferenceCount();
		m_effect_postProcessing_Distortion = nullptr;

	}
	// FXAA
	if (m_effect_FXAA)
	{
		m_effect_FXAA->DecrementReferenceCount();
		m_effect_FXAA = nullptr;

	}
	return Results::Success;
}
