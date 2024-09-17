#include "cCamera.h"

void eae6320::cCamera::Update(const float i_elapsedSecondCount_sinceLastUpdate)
{
    // Update the camera's rigid body state
    m_rigidBodyState.Update(i_elapsedSecondCount_sinceLastUpdate);
}

// Get the world-to-camera transformation matrix
eae6320::Math::cMatrix_transformation eae6320::cCamera::GetWorldToCameraTransform() const
{
    // Use the rigid body's orientation and position to create a world-to-camera transform
    return eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(m_rigidBodyState.orientation, m_rigidBodyState.position);
}

// Get the camera-to-projected transformation matrix
eae6320::Math::cMatrix_transformation eae6320::cCamera::GetCameraToProjectedTransform() const
{
    // Create the perspective projection matrix
    return eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(
        m_verticalFieldOfView_inRadians, m_aspectRatio, m_z_nearPlane, m_z_farPlane);
}

// Set the position of the camera
void eae6320::cCamera::SetPosition(const eae6320::Math::sVector& i_position)
{
    m_rigidBodyState.position = i_position;
}

// Set the orientation of the camera
void eae6320::cCamera::SetOrientation(const eae6320::Math::cQuaternion& i_orientation)
{
    m_rigidBodyState.orientation = i_orientation;
}

void eae6320::cCamera::SetProjectionParameters(const float i_verticalFieldOfView_inRadians,
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
const eae6320::Math::sVector& eae6320::cCamera::GetPosition() const
{
    return m_rigidBodyState.position;
}

// Get the orientation of the camera
const eae6320::Math::cQuaternion& eae6320::cCamera::GetOrientation() const
{
    return m_rigidBodyState.orientation;
}