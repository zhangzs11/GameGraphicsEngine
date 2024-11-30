// Includes
//=========

#include "../cCascadedShadowManager.h"
#include "../cCamera.h"

eae6320::cResult eae6320::Graphics::cCascadedShadowManager::InitResource()
{
    return Results::Success;
}

void eae6320::Graphics::cCascadedShadowManager::UpdateFrame(const eae6320::Graphics::cCamera& viewerCamera,
                                                            const eae6320::Graphics::cCamera& lightCamera,
                                                            const DirectX::BoundingBox& sceneBoundingBox,
                                                            const float i_elapsedSecondCount_sinceLastUpdate)
{
    Math::cMatrix_transformation ViewerProj = viewerCamera.GetCameraToProjectedTransform();
    Math::cMatrix_transformation ViewerView = viewerCamera.GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastUpdate);
    Math::cMatrix_transformation LightView = lightCamera.GetWorldToCameraTransform(i_elapsedSecondCount_sinceLastUpdate);
    Math::cMatrix_transformation ViewerInvView = ViewerView.GetInverse();

    float frustumIntervalBegin, frustumIntervalEnd;
    DirectX::XMVECTOR lightCameraOrthographicMinVec;     // AABB vMin in light space
    DirectX::XMVECTOR lightCameraOrthographicMaxVec;     // AABB vMax in light space
    float cameraNearFarRange = viewerCamera.m_z_farPlane - viewerCamera.m_z_nearPlane;

    DirectX::XMVECTOR worldUnitsPerTexelVec = DirectX::g_XMZero;

    //
    // Calculate the orthogonal projection matrix in Light space for each cascade
    //
    for (int cascadeIndex = 0; cascadeIndex < m_CascadeLevels; ++cascadeIndex)
    {
        // Calculate the frustum interval covered by the current cascade.
        // We measure the cascade's coverage range along the Z-axis by its minimum/maximum distance.
        if (m_SelectedCascadesFit == FitProjection::FitProjection_ToCascade)
        {
            // Since we want the orthographic projection matrix to tightly fit around the cascade,
            // we set the beginning of the frustum interval to the end of the previous cascade's interval.
            if (cascadeIndex == 0)
                frustumIntervalBegin = 0.0f;
            else
                frustumIntervalBegin = m_CascadePartitionsPercentage[cascadeIndex - 1];
        }
        else
        {
            // In FIT_PROJECTION_TO_SCENE, these cascades overlap each other.
            // For example, cascades 1-8 cover interval 1,
            // while cascades 2-8 cover interval 2.
            frustumIntervalBegin = 0.0f;
        }

        // the Z begin-end of the frustum
        frustumIntervalEnd = m_CascadePartitionsPercentage[cascadeIndex];
        frustumIntervalBegin = frustumIntervalBegin * cameraNearFarRange;
        frustumIntervalEnd = frustumIntervalEnd * cameraNearFarRange;

        DirectX::XMFLOAT3 viewerFrustumPoints[8];
        DirectX::BoundingFrustum viewerFrustum(ViewerProj.ToXMMATRIX());
        viewerFrustum.Near = frustumIntervalBegin;
        viewerFrustum.Far = frustumIntervalEnd;

        // Make the frustum from LOCAL space to WORLD space, and then LIGHT space
        viewerFrustum.Transform(viewerFrustum, ViewerInvView.ToXMMATRIX() * LightView.ToXMMATRIX());
        viewerFrustum.GetCorners(viewerFrustumPoints);
        // Calculate the AABB, vMax, vMin of frustum in LIGHT space
        DirectX::BoundingBox viewerFrustumBox;
        DirectX::BoundingBox::CreateFromPoints(viewerFrustumBox, 8, viewerFrustumPoints, sizeof(DirectX::XMFLOAT3));
        lightCameraOrthographicMaxVec = DirectX::XMVectorAdd(
            DirectX::XMLoadFloat3(&viewerFrustumBox.Center),
            DirectX::XMLoadFloat3(&viewerFrustumBox.Extents)
        );
        lightCameraOrthographicMinVec = DirectX::XMVectorSubtract(
            DirectX::XMLoadFloat3(&viewerFrustumBox.Center),
            DirectX::XMLoadFloat3(&viewerFrustumBox.Extents)
        );

        // This code can eliminate the flickering effect on shadow edges caused by changed in lighting or camera perspective
        if (m_FixedSizeFrustumAABB)
        {

        }

        // Based on the size of the PCF kernel, we calculate a boundary expansion value to slightlu enlarge the bounding box.
        // The uniform scaling will not affect the previously fixed-size AABB
        {
            float scaleDuetoBlur = m_PCFKernelSize / (float)m_ShadowSize;
            DirectX::XMVECTORF32 scaleDuetoBlurVec = { {scaleDuetoBlur, scaleDuetoBlur, 0.0f, 0.0f} };

            DirectX::XMVECTOR borderOffsetVec = DirectX::XMVectorSubtract(lightCameraOrthographicMaxVec, lightCameraOrthographicMinVec);
            borderOffsetVec = DirectX::XMVectorMultiply(borderOffsetVec, DirectX::g_XMOneHalf);
            borderOffsetVec = DirectX::XMVectorMultiply(borderOffsetVec, scaleDuetoBlurVec);

            lightCameraOrthographicMaxVec = DirectX::XMVectorAdd(lightCameraOrthographicMaxVec, borderOffsetVec);
            lightCameraOrthographicMinVec = DirectX::XMVectorSubtract(lightCameraOrthographicMinVec, borderOffsetVec);
        }

        if (m_MoveLightTexelSize)
        {

        }


        float nearPlane = 0.0f;
        float farPlane = 0.0f;

        // Transform the corner points of the scene AABB into light space
        DirectX::XMVECTOR sceneAABBPointsLightSpace[8]{};
        {
            DirectX::XMFLOAT3 corners[8];
            sceneBoundingBox.GetCorners(corners);
            for (int i = 0; i < 8; ++i)
            {
                DirectX::XMVECTOR v = XMLoadFloat3(corners + i);
                sceneAABBPointsLightSpace[i] = DirectX::XMVector3Transform(v, LightView.ToXMMATRIX());
            }
        }

        if (m_SelectedNearFarFit == FitNearFar::FitNearFar_ZeroOne)
        {
            nearPlane = 0.1f;
            farPlane = 1000.0f;
        }

        if (m_SelectedNearFarFit == FitNearFar::FitNearFar_CascadeAABB)
        {
            nearPlane = DirectX::XMVectorGetZ(lightCameraOrthographicMinVec);
            farPlane = DirectX::XMVectorGetZ(lightCameraOrthographicMaxVec);
        }
        else if (m_SelectedNearFarFit == FitNearFar::FitNearFar_SceneAABB)
        {

        }
        else if (m_SelectedNearFarFit == FitNearFar::FitNearFar_SceneAABB_Intersection)
        {

        }


        // m_ShadowProj
        // Orthographic project matrix
        //
        m_ShadowProj[cascadeIndex] = Math::cMatrix_transformation::CreateCameraToProjectedTransform_orthographic(
            DirectX::XMVectorGetX(lightCameraOrthographicMinVec),  // left
            DirectX::XMVectorGetX(lightCameraOrthographicMaxVec),  // right
            DirectX::XMVectorGetY(lightCameraOrthographicMinVec),  // bottom
            DirectX::XMVectorGetY(lightCameraOrthographicMaxVec),  // top
            -nearPlane,                                             // nearPlane
            -farPlane                                               // farPlane
        );


        // Orthographic AABB
        //
        lightCameraOrthographicMinVec = DirectX::XMVectorSetZ(lightCameraOrthographicMinVec, nearPlane);
        lightCameraOrthographicMaxVec = DirectX::XMVectorSetZ(lightCameraOrthographicMaxVec, farPlane);
        DirectX::BoundingBox::CreateFromPoints(m_ShadowProjBoundingBox[cascadeIndex],
            lightCameraOrthographicMinVec, lightCameraOrthographicMaxVec);

        // Partitions of Frustum every Cascade  
        m_CascadePartitionsFrustum[cascadeIndex] = frustumIntervalEnd;
    }

}

void eae6320::Graphics::cCascadedShadowManager::ComputeNearAndFar(float& outNearPlane, float& outFarPlane,
                                                                  eae6320::Math::sVector4 lightCameraOrthographicMinVec,
                                                                  eae6320::Math::sVector4 lightCameraOrthographicMaxVec,
                                                                  eae6320::Math::sVector4 pointsInCameraView[])
{

}