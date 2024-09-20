// Includes
//=========

#include "cQuaternion.h"

#include "sVector.h"

#include <cmath>
#include <Engine/Asserts/Asserts.h>

// Static Data
//============

namespace
{
	constexpr auto s_epsilon = 1.0e-9f;
}

// Interface
//==========

// Normalization
//--------------

void eae6320::Math::cQuaternion::Normalize()
{
	const auto length = std::sqrt( ( m_w * m_w ) + ( m_x * m_x ) + ( m_y * m_y ) + ( m_z * m_z ) );
	EAE6320_ASSERTF( length > s_epsilon, "Can't divide by zero" );
	const auto length_reciprocal = 1.0f / length;
	m_w *= length_reciprocal;
	m_x *= length_reciprocal;
	m_y *= length_reciprocal;
	m_z *= length_reciprocal;
}

eae6320::Math::cQuaternion eae6320::Math::cQuaternion::GetNormalized() const
{
	const auto length = std::sqrt( ( m_w * m_w ) + ( m_x * m_x ) + ( m_y * m_y ) + ( m_z * m_z ) );
	EAE6320_ASSERTF( length > s_epsilon, "Can't divide by zero" );
	const auto length_reciprocal = 1.0f / length;
	return cQuaternion( m_w * length_reciprocal, m_x * length_reciprocal, m_y * length_reciprocal, m_z * length_reciprocal );
}

eae6320::Math::cQuaternion eae6320::Math::cQuaternion::LookAt(const sVector& i_direction, const sVector& i_up)
{
    //using namespace eae6320::Math;
    // Ensure the direction is normalized
    sVector forward = i_direction.GetNormalized();
    sVector right = Cross(i_up, forward).GetNormalized();
    sVector up = Cross(forward, right);

    // Create a 3x3 rotation matrix from the axes
    float m00 = right.x, m01 = up.x, m02 = forward.x;
    float m10 = right.y, m11 = up.y, m12 = forward.y;
    float m20 = right.z, m21 = up.z, m22 = forward.z;

    // Convert the rotation matrix to a quaternion
    float trace = m00 + m11 + m22;

    float qw, qx, qy, qz;
    if (trace > 0.0f)
    {
        float s = sqrt(trace + 1.0f) * 2.0f; // s = 4 * qw
        qw = 0.25f * s;
        qx = (m21 - m12) / s;
        qy = (m02 - m20) / s;
        qz = (m10 - m01) / s;
    }
    else if ((m00 > m11) && (m00 > m22))
    {
        float s = sqrt(1.0f + m00 - m11 - m22) * 2.0f; // s = 4 * qx
        qw = (m21 - m12) / s;
        qx = 0.25f * s;
        qy = (m01 + m10) / s;
        qz = (m02 + m20) / s;
    }
    else if (m11 > m22)
    {
        float s = sqrt(1.0f + m11 - m00 - m22) * 2.0f; // s = 4 * qy
        qw = (m02 - m20) / s;
        qx = (m01 + m10) / s;
        qy = 0.25f * s;
        qz = (m12 + m21) / s;
    }
    else
    {
        float s = sqrt(1.0f + m22 - m00 - m11) * 2.0f; // s = 4 * qz
        qw = (m10 - m01) / s;
        qx = (m02 + m20) / s;
        qy = (m12 + m21) / s;
        qz = 0.25f * s;
    }

    return cQuaternion(qw, qx, qy, qz);
}

// Initialize / Clean Up
//----------------------

eae6320::Math::cQuaternion::cQuaternion( const float i_angleInRadians, const sVector i_axisOfRotation_normalized )
{
	const auto theta_half = i_angleInRadians * 0.5f;
	m_w = std::cos( theta_half );
	const auto sin_theta_half = std::sin( theta_half );
	m_x = i_axisOfRotation_normalized.x * sin_theta_half;
	m_y = i_axisOfRotation_normalized.y * sin_theta_half;
	m_z = i_axisOfRotation_normalized.z * sin_theta_half;
}
