#ifndef EAE6320_GRAPHICS_CCAMERA_H
#define EAE6320_GRAPHICS_CCAMERA_H

// Includes
//=========

#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Physics/sRigidBodyState.h>
#include <vector>

// Enum Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		enum class eCameraType
		{
			Perspective,
			Orthographic,
		};
	}
}

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cCamera
		{
		public:
			// Update
			void Update(const float i_elapsedSecondCount_sinceLastUpdate);

			Physics::sRigidBodyState& GetRigidBodyState() { return m_rigidBodyState; }

			// Get the transform matrix
			Math::cMatrix_transformation GetWorldToCameraTransform(const float i_elapsedSecondCount_sinceLastUpdate) const;
			Math::cMatrix_transformation GetCameraToProjectedTransform() const;

			// Get the frustumVertices
			std::vector<Math::sVector> CalculateFrustumVertices(float partial_z_near, float partial_z_far) const;

			// Setters for camera properties
			void SetType(const eCameraType i_type);
			void SetPosition(const Math::sVector& i_position);
			void SetOrientation(const Math::cQuaternion& i_orientation);
			void SetProjectionParameters(
				const float i_verticalFieldOfView_inRadians,
				const float i_aspectRatio,
				const float i_z_nearPlane,
				const float i_z_farPlane);

			// Getters for camera properties
			const Math::sVector& GetPosition() const;
			const Math::cQuaternion& GetOrientation() const;

			Math::sVector GetForwardDirection() const
			{
				return m_rigidBodyState.orientation.CalculateForwardDirection();
			}

			Math::sVector GetRightDirection() const
			{
				return m_rigidBodyState.orientation.CalculateRightDirection();
			}

			Math::sVector GetUpDirection() const
			{
				return m_rigidBodyState.orientation.CalculateUpDirection();
			}

		public:
			Physics::sRigidBodyState m_rigidBodyState;
			eCameraType m_type = eCameraType::Perspective;

			// Projection parameters
			float m_verticalFieldOfView_inRadians;
			float m_aspectRatio;
			float m_z_nearPlane;
			float m_z_farPlane;

			// Orthographic parameters
			float m_orthographicWidth = 5.0f;


		};
	}
}


#endif	// EAE6320_GRAPHICS_CCAMERA_H
