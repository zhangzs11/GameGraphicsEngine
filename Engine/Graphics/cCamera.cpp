#include "cCamera.h"

void eae6320::Graphics::cCamera::Update(const float i_elapsedSecondCount_sinceLastUpdate)
{
    // Update the camera's rigid body state
    m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
}

// Get the world-to-camera transformation matrix
eae6320::Math::cMatrix_transformation eae6320::Graphics::cCamera::GetWorldToCameraTransform(const float i_elapsedSecondCount_sinceLastUpdate) const
{
    // Use the rigid body's orientation and position to create a world-to-camera transform
    // return eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(m_rigidBodyState.orientation, m_rigidBodyState.position);
    return eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(m_rigidBodyState.PredictFutureOrientation(i_elapsedSecondCount_sinceLastUpdate), m_rigidBodyState.PredictFuturePosition(i_elapsedSecondCount_sinceLastUpdate));
}

// Get the camera-to-projected transformation matrix
eae6320::Math::cMatrix_transformation eae6320::Graphics::cCamera::GetCameraToProjectedTransform() const
{
    // Create the perspective projection matrix
    if (m_type == eCameraType::Perspective) {
        return eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(
            m_verticalFieldOfView_inRadians, m_aspectRatio, m_z_nearPlane, m_z_farPlane);
    }
    else {
        // Calculate orthographic projection parameters
        float width = m_orthographicWidth;
        float height = width / m_aspectRatio;

        float left = -width / 2.0f;
        float right = width / 2.0f;
        float bottom = -height / 2.0f;
        float top = height / 2.0f;

        // Create the orthographic projection matrix
        return eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_orthographic(
            left, right, bottom, top, m_z_nearPlane, m_z_farPlane);
    }
}
void eae6320::Graphics::cCamera::SetType(const eae6320::Graphics::eCameraType i_type)
{
    m_type = i_type;
}

// Set the position of the camera
void eae6320::Graphics::cCamera::SetPosition(const eae6320::Math::sVector& i_position)
{
    m_rigidBodyState.position = i_position;
}

// Set the orientation of the camera
void eae6320::Graphics::cCamera::SetOrientation(const eae6320::Math::cQuaternion& i_orientation)
{
    m_rigidBodyState.orientation = i_orientation;
}

void eae6320::Graphics::cCamera::SetProjectionParameters(const float i_verticalFieldOfView_inRadians,
    const float i_aspectRatio,
    const float i_z_nearPlane,
    const float i_z_farPlane)
{
    m_verticalFieldOfView_inRadians = i_verticalFieldOfView_inRadians;
    m_aspectRatio = i_aspectRatio;
    m_z_nearPlane = i_z_nearPlane;
    m_z_farPlane = i_z_farPlane;
}

// Get the position of the camera
const eae6320::Math::sVector& eae6320::Graphics::cCamera::GetPosition() const
{
    return m_rigidBodyState.position;
}

// Get the orientation of the camera
const eae6320::Math::cQuaternion& eae6320::Graphics::cCamera::GetOrientation() const
{
    return m_rigidBodyState.orientation;
}