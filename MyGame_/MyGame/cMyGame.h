/*
	This class is your specific game
*/

#ifndef EAE6320_CEXAMPLEGAME_H
#define EAE6320_CEXAMPLEGAME_H

// Includes
//=========
#include <vector>

#include <Engine/Application/iApplication.h>
#include <Engine/Results/Results.h>
#include <Engine/Graphics/Graphics.h>

#include "cGameObject.h"
// #include "cCamera.h"

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include "Resource Files/Resource.h"
#endif

// Class Declaration
//==================

namespace eae6320
{
	class cMyGame final : public Application::iApplication
	{
		// Inherited Implementation
		//=========================

	private:

		// Configuration
		//--------------

#if defined( EAE6320_PLATFORM_WINDOWS )
		// The main window's name will be displayed as its caption (the text that is displayed in the title bar).
		// You can make it anything that you want, but please keep the platform name and debug configuration at the end
		// so that it's easy to tell at a glance what kind of build is running.
		const char* GetMainWindowName() const final
		{
			return "Zishuai-Zhang's EAE6320 Example Game"
				" -- "
#if defined( EAE6320_PLATFORM_D3D )
				"Direct3D"
#elif defined( EAE6320_PLATFORM_GL )
				"OpenGL"
#endif
#ifdef _DEBUG
				" -- Debug"
#endif
			;
		}
		// Window classes are almost always identified by name;
		// there is a unique "ATOM" associated with them,
		// but in practice Windows expects to use the class name as an identifier.
		// If you don't change the name below
		// your program could conceivably have problems if it were run at the same time on the same computer
		// as one of your classmate's.
		// You don't need to worry about this for our class,
		// but if you ever ship a real project using this code as a base you should set this to something unique
		// (a generated GUID would be fine since this string is never seen)
		const char* GetMainWindowClassName() const final { return "Zishuai-Zhang's EAE6320 Example Main Window Class"; }
		// The following three icons are provided:
		//	* IDI_EAEGAMEPAD
		//	* IDI_EAEALIEN
		//	* IDI_VSDEFAULT_LARGE / IDI_VSDEFAULT_SMALL
		// If you want to try creating your own a convenient website that will help is: http://icoconvert.com/
		const WORD* GetLargeIconId() const final { static constexpr WORD iconId_large = IDI_EAEGAMEPAD; return &iconId_large; }
		const WORD* GetSmallIconId() const final { static constexpr WORD iconId_small = IDI_EAEGAMEPAD; return &iconId_small; }

		void GetDefaultInitialResolution(uint16_t& o_width, uint16_t& o_height) const final
		{
			o_width = 1024;
			o_height = 768;
		}
#endif

		// Run
		//----

		void UpdateBasedOnInput() final;
		void UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) final;
		void UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) final;
		void SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate) final;

		void SubmitGameObjectToGraphics(cGameObject& i_gameObject, const float i_elapsedSecondCount_sinceLastSimulationUpdate);
		void SubmitCameraToGraphics(eae6320::Graphics::cCamera& i_camera, const float i_elapsedSecondCount_sinceLastSimulationUpdate);
		void SubmitLightDataToGraphics(
									   std::vector<eae6320::Graphics::sDirectionalLight>& i_directionalLights,
									   std::vector<eae6320::Graphics::sPointLight>& i_pointLights,
									   std::vector<eae6320::Graphics::sSpotLight>& i_spotLights,
			                           const float i_elapsedSecondCount_systemTime);

		void SubmitShadowDataToGraphics(eae6320::Graphics::ShadowEffect* i_Shadoweffect,
			eae6320::Graphics::sDirectionalLight& i_dirL);

		void SubmitSkyboxDataToGraphics(eae6320::Graphics::SkyboxEffect* i_skuboxeffect,
			eae6320::Graphics::cMesh* i_cubemesh);

		void SubmitPostProcessingDataToGraphics(eae6320::Graphics::PostProcessingEffect* i_postProcessingeffect);
		void SubmitFXAADataToGraphics(eae6320::Graphics::PostProcessingEffect* i_FXAAffect,
			float g_TexelSize_x, float g_TexelSize_y,
			float g_QualitySubPix,
			float g_QualityEdgeThreshold,
			float g_QualityEdgeThresholdMin);

		// Initialize / Clean Up
		//----------------------

		cResult Initialize() final;
		cResult CleanUp() final;

		// Mesh / Effect
		//----------------------

		Graphics::cMesh* m_mesh_plane = nullptr;
		Graphics::cMesh* m_mesh_tree = nullptr;
		Graphics::cMesh* m_mesh_cube = nullptr;
		Graphics::cMesh* m_mesh_table = nullptr;
		Graphics::cMesh* m_mesh_marblebust = nullptr;
		Graphics::cMesh* m_mesh_marblebust_big = nullptr;
		Graphics::cMesh* m_mesh_wall = nullptr;

		Graphics::LightingEffect* m_effect_tree = nullptr;
		Graphics::LightingEffect* m_effect_plane = nullptr;
		Graphics::LightingEffect* m_effect_table = nullptr;
		Graphics::LightingEffect* m_effect_marblebust = nullptr;
		Graphics::LightingEffect* m_effect_wall = nullptr;

		Graphics::SkyboxEffect* m_effect_skybox = nullptr;
		Graphics::ShadowEffect* m_effect_shadowMap = nullptr;

		Graphics::PostProcessingEffect* m_effect_postProcessing_Default = nullptr;
		Graphics::PostProcessingEffect* m_effect_postProcessing_Distortion = nullptr;
		Graphics::PostProcessingEffect* m_effect_FXAA = nullptr;

		// FXAA
		float m_FXAA_QualitySubPix;
		float m_FXAA_QualityEdgeThreshold;
		float m_FXAA_QualityEdgeThresholdMin;


		std::vector<eae6320::Graphics::sDirectionalLight> m_directionalLights;
		std::vector<eae6320::Graphics::sPointLight> m_pointLights;
		std::vector<eae6320::Graphics::sSpotLight> m_spotLights;


		// Game Objects
		//----------------------
		cGameObject m_gameObject_plane[60];
		cGameObject m_gameObject_tree;
		cGameObject m_gameObject_tree2;
		cGameObject m_gameObject_table;
		cGameObject m_gameObject_marblebust;
		cGameObject m_gameObject_marblebust_big;
		cGameObject m_gameObject_wall;

		cGameObject m_gameObject_skybox;

		Graphics::cCamera m_camera;
		float m_backgroundColor[4] = { 0.0f, 0.8f, 0.2f, 1.0f };

		// Game States
		//----------------------
		bool ifUpPressed = false;
		bool ifDownPressed = false;
		bool ifLeftPressed = false;
		bool ifRightPressed = false;
		Graphics::PostProcessingEffect* m_current_postprocess_effect = nullptr;

	};
}

// Result Definitions
//===================

namespace eae6320
{
	namespace Results
	{
		namespace Application
		{
			// You can add specific results for your game here:
			//	* The System should always be Application
			//	* The __LINE__ macro is used to make sure that every result has a unique ID.
			//		That means, however, that all results _must_ be defined in this single file
			//		or else you could have two different ones with equal IDs.
			//	* Note that you can define multiple Success codes.
			//		This can be used if the caller may want to know more about how a function succeeded.
			constexpr cResult ExampleResult( IsFailure, eSystem::Application, __LINE__, Severity::Default );
		}
	}
}

#endif	// EAE6320_CEXAMPLEGAME_H
