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
				// Main Rendering
				// --------------------------------------

				Math::cMatrix_transformation g_transform_worldToCamera;
				Math::cMatrix_transformation g_transform_cameraToProjected;

				float g_elapsedSecondCount_systemTime = 0.0f;
				float g_elapsedSecondCount_simulationTime = 0.0f;
				float g_padding1[2];

				sDirectionalLight g_DirLight;
				sPointLight g_PointLight[10];
				sSpotLight g_SpotLight[10];

				Math::sVector g_EyePosW;
				float g_padding2;

				Math::cMatrix_transformation g_ShadowView;
				Math::sVector4 g_CascadeOffset[8];
				Math::sVector4 g_CascadeScale[8];
				int g_VisualizeCascades;
				int g_PCFBlurForLoopStart;
				int g_PCFBlurForLoopEnd;
				int g_Pad;

				float g_MinBorderPadding;
				float g_MaxBorderPadding;
				float g_ShadowBias;
				float g_CascadeBlendArea;

				float g_TexelSize_shadowMap;
				float g_padding3[3];

				Math::sVector4 g_CascadeFrustumsEyeSpaceDepthsFloat;
				Math::sVector4 g_CascadeFrustumsEyeSpaceDepthsFloat4[4];

				// FXAA
				// --------------------------------------
				float g_TexelSize_x;
				float g_TexelSize_y;

				// Controls sharpness level
				// 1.00 - Soft
				// 0.75 - Default filter value
				// 0.50 - Sharper, removes less subpixel aliasing
				// 0.25 - Almost off
				// 0.00 - Completely off
				float g_QualitySubPix;

				// Threshold control for required local contrast
				// 0.333 - Very low (faster)
				// 0.250 - Low quality
				// 0.166 - Default
				// 0.125 - High quality
				// 0.063 - Very high (slower)
				float g_QualityEdgeThreshold;

				// Threshold for ignoring dark areas
				// 0.0833 - Default
				// 0.0625 - Slightly faster
				// 0.0312 - Slower
				float g_QualityEdgeThresholdMin;

				float g_padding4[3];
			};

			struct sFrame_Deferred
			{
				Math::cMatrix_transformation g_transform_worldToCamera;
				Math::cMatrix_transformation g_transform_cameraToProjected;

				float g_elapsedSecondCount_systemTime;
				float g_elapsedSecondCount_simulationTime;
				float g_padding1[2];

				Math::sVector4 g_CameraNearFar;
			};

			// Data that is constant for a single draw call
			struct sDrawCall
			{
				Math::cMatrix_transformation g_transform_localToWorld;
				Math::cMatrix_transformation g_transform_localToWorld_Inv_Transpose;

				sMaterial g_Material;
			};

			struct sDrawCall_shadowMap
			{
				Math::cMatrix_transformation g_World;
				Math::cMatrix_transformation g_View;
				Math::cMatrix_transformation g_Proj;
			};
		}
	}
}

#endif	// EAE6320_GRAPHICS_CONSTANTBUFFERFORMATS_H
