#ifndef EAE6320_GRAPHICS_CCASCADEDSHADOWMANAGER_H
#define EAE6320_GRAPHICS_CCASCADEDSHADOWMANAGER_H

// Includes
//=========

#include <cstdint>
#include <Engine/Results/Results.h>
#include <Engine/Math/sVector.h>
#include <Engine/Math/cMatrix_transformation.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "Direct3D/Includes.h"

// Forward Declarations
//=====================

namespace eae6320
{
    namespace Graphics
    {
        class cCamera;
        class cView_DSV_Array;
        class cView_SRV_Array;
    }
}


// Enum Declaration
//==================

enum class CascadeSelection
{
    CascadeSelection_Map,
    CascadeSelection_Interval
};

enum class CameraSelection
{
    CameraSelection_Eye,
    CameraSelection_Light,
    CameraSelection_Cascade1,
    CameraSelection_Cascade2,
    CameraSelection_Cascade3,
    CameraSelection_Cascade4,
    CameraSelection_Cascade5,
    CameraSelection_Cascade6,
    CameraSelection_Cascade7,
    CameraSelection_Cascade8,
};

enum class FitNearFar
{
    FitNearFar_ZeroOne,
    FitNearFar_CascadeAABB,
    FitNearFar_SceneAABB,
    FitNearFar_SceneAABB_Intersection
};

enum class FitProjection
{
    FitProjection_ToCascade,
    FitProjection_ToScene
};


// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cCascadedShadowManager
		{
        public:
            cResult InitResource();

            void UpdateFrame(
                const cCamera& viewerCamera,
                const cCamera& lightCamera,
                const DirectX::BoundingBox& sceneBouindingBox,
                const float i_elapsedSecondCount_sinceLastUpdate);

        public:
            //
            // SET UP DATA
            //

            int         m_ShadowSize = 1024;
            int         m_CascadeLevels = 4;

            float		m_CascadePartitionsPercentage[4]{      
                0.2f, 0.4f, 0.6f, 1.0f
            };
            int		    m_PCFKernelSize = 5;                    
            float       m_PCFDepthOffset = 0.001f;              
            bool        m_DerivativeBasedOffset = false;        
            bool        m_BlendBetweenCascades = false;
            float       m_BlendBetweenCascadesRange = 0.2f;     

            bool        m_FixedSizeFrustumAABB = false;
            bool        m_MoveLightTexelSize = false;

            CameraSelection     m_SelectedCamera = CameraSelection::CameraSelection_Eye;
            FitProjection       m_SelectedCascadesFit = FitProjection::FitProjection_ToScene;
            FitNearFar          m_SelectedNearFarFit = FitNearFar::FitNearFar_ZeroOne;
            CascadeSelection    m_SelectedCascadeSelection = CascadeSelection::CascadeSelection_Map;

        private:
            void ComputeNearAndFar(float& outNearPlane, float& outFarPlane,
                Math::sVector4 lightCameraOrthographicMinVec,
                Math::sVector4 lightCameraOrthographicMaxVec,
                Math::sVector4 pointsInCameraView[]);

        public:
            float	                        m_CascadePartitionsFrustum[8]{};    
            Math::cMatrix_transformation    m_ShadowProj[8]{};                  
            DirectX::BoundingBox            m_ShadowProjBoundingBox[8]{};       
            D3D11_VIEWPORT                  m_ShadowViewport{};                 

            cView_DSV_Array* m_CSM_DSV_Array;
            cView_SRV_Array* m_CSM_SRV;

		};
	}

}

#endif // EAE6320_GRAPHICS_CCASCADEDSHADOWMANAGER_H
