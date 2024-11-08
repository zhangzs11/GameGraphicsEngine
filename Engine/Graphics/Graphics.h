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
#include "MyEffects/LightingEffect.d3d.h"
#include "MyEffects/ShadowEffect.d3d.h"
#include "MyEffects/SkyboxEffect.d3d.h"

#include <cstdint>
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

		void SubmitBackgroundColor(const float i_backgroundColor[4]);

		void SubmitMatrixLocalToWorld(const eae6320::Math::cMatrix_transformation& i_transform_localToWorld);

		void SubmitCameraData(const eae6320::Math::cMatrix_transformation& i_transform_worldToCamera, 
			                  const eae6320::Math::cMatrix_transformation& i_transform_cameraToProjected, 
			                  const Math::sVector& i_eyePosW);

		void SubmitLightData(const sDirectionalLight& i_directionalLight,
			                 const sPointLight& i_pointLight,
			                 const sSpotLight& i_spotLight);

		void SubmitMaterial(const sMaterial& i_material);
		void SubmitShadowData(ShadowEffect* i_Shadoweffect,
			                  const eae6320::Math::cMatrix_transformation& i_transform_worldToLightCamera,
			                  const eae6320::Math::cMatrix_transformation& i_transform_LightcameraToProjected, 
			                  const eae6320::Math::cMatrix_transformation& i_ShadowTransform);

		void SubmitSkyboxData(SkyboxEffect* i_skuboxeffect,
			                  cMesh* i_cubemesh);

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
