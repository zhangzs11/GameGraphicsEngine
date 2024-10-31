/*
	This file defines the layout of the constant data
	that the CPU sends to the GPU

	These must exactly match the constant buffer definitions in shader programs.
*/

#ifndef EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H
#define EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H

// Includes
//=========

#include "Configuration.h"
#include "sLight.h"

#include <Engine/Math/cMatrix_transformation.h>


// Format Definitions
//===================

namespace eae6320
{
	namespace Graphics
	{
		namespace ConstantBufferFormats
		{
			// Data that is constant for every frame
			struct sFrame
			{
				Math::cMatrix_transformation g_transform_worldToCamera;
				Math::cMatrix_transformation g_transform_cameraToProjected;

				float g_elapsedSecondCount_systemTime = 0.0f;
				float g_elapsedSecondCount_simulationTime = 0.0f;
				// For float4 alignment
				float g_padding1[2];

				sDirectionalLight g_DirLight;
				sPointLight g_PointLight;
				sSpotLight g_SpotLight;

				// For float4 alignment
				Math::sVector g_EyePosW;
				float g_padding2;
			};

			// Data that is constant for a single draw call
			struct sDrawCall
			{
				Math::cMatrix_transformation g_transform_localToWorld;
				Math::cMatrix_transformation g_transform_localToWorld_Inv_Transpose;

				sMaterial g_Material;
			};
		}
	}
}

#endif	// EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H
