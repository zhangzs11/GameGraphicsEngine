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

        auto frustumVertices = viewerCamera.CalculateFrustumVertices(frustumIntervalBegin, frustumIntervalEnd);
        // Make the frustum from LOCAL space to WORLD space, and then LIGHT space
        std::vector<Math::sVector4> lightFrustumVertices;
        for (const auto& vertex : frustumVertices)
        {
            Math::sVector4 vertex4(vertex.x, vertex.y, vertex.z, 1.0f);

            Math::sVector4 worldSpaceVertex = ViewerInvView * vertex4;

            Math::sVector4 lightSpaceVertex = LightView * worldSpaceVertex;

            lightFrustumVertices.push_back(lightSpaceVertex);
        }

        DirectX::XMVECTOR lightCameraOrthographicMinVec = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0.0f);
        DirectX::XMVECTOR lightCameraOrthographicMaxVec = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0.0f);


        // Calculate the AABB, vMax, vMin of frustum in LIGHT space
        for (const auto& vertex : lightFrustumVertices)
        {
            DirectX::XMVECTOR v = DirectX::XMVectorSet(vertex.x, vertex.y, vertex.z, 1.0f);
            lightCameraOrthographicMinVec = DirectX::XMVectorMin(lightCameraOrthographicMinVec, v);
            lightCameraOrthographicMaxVec = DirectX::XMVectorMax(lightCameraOrthographicMaxVec, v);
        }

        float left = DirectX::XMVectorGetX(lightCameraOrthographicMinVec);
        float right = DirectX::XMVectorGetX(lightCameraOrthographicMaxVec);
        float bottom = DirectX::XMVectorGetY(lightCameraOrthographicMinVec);
        float top = DirectX::XMVectorGetY(lightCameraOrthographicMaxVec);

        // This code can eliminate the flickering effect on shadow edges caused by changed in lighting or camera perspective
        if (m_FixedSizeFrustumAABB)
        {

        }

        // Based on the size of the PCF kernel, we calculate a boundary expansion value to slightlu enlarge the bounding box.
        // The uniform scaling will not affect the previously fixed-size AABB
        {
            /*float scaleDuetoBlur = m_PCFKernelSize / (float)m_ShadowSize;
            DirectX::XMVECTORF32 scaleDuetoBlurVec = { {scaleDuetoBlur, scaleDuetoBlur, 0.0f, 0.0f} };

            DirectX::XMVECTOR borderOffsetVec = DirectX::XMVectorSubtract(lightCameraOrthographicMaxVec, lightCameraOrthographicMinVec);
            borderOffsetVec = DirectX::XMVectorMultiply(borderOffsetVec, DirectX::g_XMOneHalf);
            borderOffsetVec = DirectX::XMVectorMultiply(borderOffsetVec, scaleDuetoBlurVec);*/

            // lightCameraOrthographicMaxVec = DirectX::XMVectorAdd(lightCameraOrthographicMaxVec, borderOffsetVec);
            // lightCameraOrthographicMinVec = DirectX::XMVectorSubtract(lightCameraOrthographicMinVec, borderOffsetVec);
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
            nearPlane = 10.0f;
            farPlane = 500.0f;
        }

        if (m_SelectedNearFarFit == FitNearFar::FitNearFar_CascadeAABB)
        {
            /*nearPlane = DirectX::XMVectorGetZ(lightCameraOrthographicMinVec);
            farPlane = DirectX::XMVectorGetZ(lightCameraOrthographicMaxVec);*/
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
            left,
            right,
            bottom,
            top,
            nearPlane,                                             // nearPlane
            farPlane                                               // farPlane
        );

        // Orthographic AABB
        //
        /*lightCameraOrthographicMinVec = DirectX::XMVectorSetZ(lightCameraOrthographicMinVec, -nearPlane);
        lightCameraOrthographicMaxVec = DirectX::XMVectorSetZ(lightCameraOrthographicMaxVec, -farPlane);
        DirectX::BoundingBox::CreateFromPoints(m_ShadowProjBoundingBox[cascadeIndex],
            lightCameraOrthographicMinVec, lightCameraOrthographicMaxVec);*/

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