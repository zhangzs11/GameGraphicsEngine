/*
	This file declares the external interface for the graphics system
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Includes
//=========

#include "Configuration.h"
#include "cMesh.h"
#include "cEffect.h"
#include "sLight.h"
#include "cCamera.h"
#include "MyEffects/LightingEffect.d3d.h"
#include "MyEffects/ShadowEffect.d3d.h"
#include "MyEffects/SkyboxEffect.d3d.h"
#include "cCascadedShadowManager.h"

#include <cstdint>
#include <vector>
#include <Engine/Results/Results.h>
#include <Engine/Math/cMatrix_transformation.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include <Engine/Windows/Includes.h>
#endif

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		// Submission
		//-----------

		// These functions should be called from the application (on the application loop thread)

		// As the class progresses you will add your own functions for submitting data,
		// but the following is an example (that gets called automatically)
		// of how the application submits the total elapsed times
		// for the frame currently being submitted
		void SubmitElapsedTime( const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime );

		void SubmitMeshEffectPair(cMesh* i_mesh, LightingEffect* i_effect);

		void SubmitMatrixLightSpaceLocalToProjected(size_t cascadeIndex,
			const eae6320::Math::cMatrix_transformation& LightSpace_Local_Project);

		void SubmitBackgroundColor(const float i_backgroundColor[4]);

		void SubmitMatrixLocalToWorld(const eae6320::Math::cMatrix_transformation& i_transform_localToWorld);

		void SubmitCameraData(const eae6320::Math::cMatrix_transformation& i_transform_worldToCamera, 
			                  const eae6320::Math::cMatrix_transformation& i_transform_cameraToProjected, 
			                  const Math::sVector& i_eyePosW);

		void SubmitLightData(const std::vector<eae6320::Graphics::sDirectionalLight>& i_directionalLights,
			                 const std::vector<eae6320::Graphics::sPointLight>& i_pointLights,
			                 const std::vector<eae6320::Graphics::sSpotLight>& i_spotLights);

		void SubmitMaterial(const sMaterial& i_material);
		
		void SubmitShadowData(
			ShadowEffect* i_Shadoweffect,
			const eae6320::Math::cMatrix_transformation& i_transform_shadowView,
			const eae6320::Math::sVector4 i_cascadeOffsets[4],
			const eae6320::Math::sVector4 i_cascadeScales[4],
			const float i_cascadeFrustumsEyeSpaceDepths[4],
			int i_visualizeCascades,
			int i_pcfBlurForLoopStart,
			int i_pcfBlurForLoopEnd,
			float i_minBorderPadding,
			float i_maxBorderPadding,
			float i_shadowBias,
			float i_cascadeBlendArea,
			float i_texelSize);

		void SubmitSkyboxData(SkyboxEffect* i_skuboxeffect,
			                  cMesh* i_cubemesh);

		void SubmitPostProcessingData(PostProcessingEffect* i_postprocessEffect);

		void SubmitFXAAData(PostProcessingEffect* i_FXAAEffect,
			                float g_TexelSize_x, float g_TexelSize_y,
			                float g_QualitySubPix,
			                float g_QualityEdgeThreshold,
			                float g_QualityEdgeThresholdMin);

		// When the application is ready to submit data for a new frame
		// it should call this before submitting anything
		// (or, said another way, it is not safe to submit data for a new frame
		// until this function returns successfully)
		cResult WaitUntilDataForANewFrameCanBeSubmitted( const unsigned int i_timeToWait_inMilliseconds );
		// When the application has finished submitting data for a frame
		// it must call this function
		cResult SignalThatAllDataForAFrameHasBeenSubmitted();

		// Render
		//-------

		// This is called (automatically) from the main/render thread.
		// It will render a submitted frame as soon as it is ready
		// (i.e. as soon as SignalThatAllDataForAFrameHasBeenSubmitted() has been called)
		void RenderFrame();

		// Initialize / Clean Up
		//----------------------

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow = NULL;
	#if defined( EAE6320_PLATFORM_D3D )
			uint16_t resolutionWidth = 0, resolutionHeight = 0;
	#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication = NULL;
	#endif
#endif
		};

		cResult Initialize( const sInitializationParameters& i_initializationParameters );
		cResult CleanUp();
	}
}

#endif	// EAE6320_GRAPHICS_H
