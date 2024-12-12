// Includes
//=========

#include "cMatrix_transformation.h"

#include "cQuaternion.h"
#include "sVector.h"

#include <cmath>
#include <DirectXMath.h>
// Interface
//==========
const DirectX::XMMATRIX eae6320::Math::cMatrix_transformation::ToXMMATRIX() const
{
	return DirectX::XMMATRIX(
		m_00, m_01, m_02, m_03,
		m_10, m_11, m_12, m_13,
		m_20, m_21, m_22, m_23,
		m_30, m_31, m_32, m_33
	);

}
const eae6320::Math::cMatrix_transformation eae6320::Math::cMatrix_transformation::GetInverse() const
{
	DirectX::XMMATRIX dxMatrix = this->ToXMMATRIX();
	DirectX::XMMATRIX inverseMatrix = DirectX::XMMatrixInverse(nullptr, dxMatrix);
	cMatrix_transformation result;
	result.m_00 = inverseMatrix.r[0].m128_f32[0];
	result.m_01 = inverseMatrix.r[0].m128_f32[1];
	result.m_02 = inverseMatrix.r[0].m128_f32[2];
	result.m_03 = inverseMatrix.r[0].m128_f32[3];

	result.m_10 = inverseMatrix.r[1].m128_f32[0];
	result.m_11 = inverseMatrix.r[1].m128_f32[1];
	result.m_12 = inverseMatrix.r[1].m128_f32[2];
	result.m_13 = inverseMatrix.r[1].m128_f32[3];

	result.m_20 = inverseMatrix.r[2].m128_f32[0];
	result.m_21 = inverseMatrix.r[2].m128_f32[1];
	result.m_22 = inverseMatrix.r[2].m128_f32[2];
	result.m_23 = inverseMatrix.r[2].m128_f32[3];

	result.m_30 = inverseMatrix.r[3].m128_f32[0];
	result.m_31 = inverseMatrix.r[3].m128_f32[1];
	result.m_32 = inverseMatrix.r[3].m128_f32[2];
	result.m_33 = inverseMatrix.r[3].m128_f32[3];

	return result;
}

const eae6320::Math::cMatrix_transformation eae6320::Math::cMatrix_transformation::GetTranspose() const
{
	cMatrix_transformation result;
	result.m_00 = m_00;
	result.m_01 = m_10;
	result.m_02 = m_20;
	result.m_03 = m_30;

	result.m_10 = m_01;
	result.m_11 = m_11;
	result.m_12 = m_21;
	result.m_13 = m_31;

	result.m_20 = m_02;
	result.m_21 = m_12;
	result.m_22 = m_22;
	result.m_23 = m_32;

	result.m_30 = m_03;
	result.m_31 = m_13;
	result.m_32 = m_23;
	result.m_33 = m_33;

	return result;
}

// Access
//-------

const eae6320::Math::sVector& eae6320::Math::cMatrix_transformation::GetRightDirection() const
{
	return *reinterpret_cast<const sVector*>( &m_00 );
}

const eae6320::Math::sVector& eae6320::Math::cMatrix_transformation::GetUpDirection() const
{
	return *reinterpret_cast<const sVector*>( &m_01 );
}

const eae6320::Math::sVector& eae6320::Math::cMatrix_transformation::GetBackDirection() const
{
	return *reinterpret_cast<const sVector*>( &m_02 );
}

const eae6320::Math::sVector& eae6320::Math::cMatrix_transformation::GetTranslation() const
{
	return *reinterpret_cast<const sVector*>( &m_03 );
}

// Camera
//-------

eae6320::Math::cMatrix_transformation eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(
	const float i_verticalFieldOfView_inRadians,
	const float i_aspectRatio,
	const float i_z_nearPlane, const float i_z_farPlane )
{
	const auto yScale = 1.0f / std::tan( i_verticalFieldOfView_inRadians * 0.5f );
	const auto xScale = yScale / i_aspectRatio;
#if defined( EAE6320_PLATFORM_D3D )
	const auto zDistanceScale = i_z_farPlane / (i_z_nearPlane - i_z_farPlane);
	return cMatrix_transformation(
		xScale,                              0.0f,                                 0.0f,                           0.0f,
		0.0f,                                yScale,                               0.0f,                           0.0f,
		0.0f,                                0.0f,                                 zDistanceScale,                 -1.0f,
		0.0f,                                0.0f,                                 i_z_nearPlane * zDistanceScale, 0.0f);
#elif defined( EAE6320_PLATFORM_GL )
	const auto zDistanceScale = 1.0f / ( i_z_nearPlane - i_z_farPlane );
	return cMatrix_transformation(
		xScale, 0.0f, 0.0f, 0.0f,
		0.0f, yScale, 0.0f, 0.0f,
		0.0f, 0.0f, ( i_z_nearPlane + i_z_farPlane ) * zDistanceScale, -1.0f,
		0.0f, 0.0f, ( 2.0f * i_z_nearPlane * i_z_farPlane ) * zDistanceScale, 0.0f );
#endif
}

eae6320::Math::cMatrix_transformation eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_orthographic(
	const float i_left, const float i_right,
	const float i_bottom, const float i_top,
	const float i_z_nearPlane, const float i_z_farPlane)
{
	const auto xScale = 2.0f / (i_right - i_left);
	const auto yScale = 2.0f / (i_top - i_bottom);
#if defined( EAE6320_PLATFORM_D3D )
	const auto zScale = 1.0f / (i_z_nearPlane - i_z_farPlane);
	return cMatrix_transformation(
		xScale,                                0.0f,                                  0.0f,                      0.0f,
		0.0f,                                  yScale,                                0.0f,                      0.0f,
		0.0f,							       0.0f,                                  zScale,                    0.0f,
		-(i_left + i_right) * xScale * 0.5f,   -(i_top + i_bottom) * yScale * 0.5f,   i_z_nearPlane * zScale,    1.0f);
#elif defined( EAE6320_PLATFORM_GL )
	const auto zScale = 2.0f / (i_z_nearPlane - i_z_farPlane);
	return cMatrix_transformation(
		xScale, 0.0f, 0.0f, 0.0f,
		0.0f, yScale, 0.0f, 0.0f,
		0.0f, 0.0f, zScale, 0.0f,
		-(i_left + i_right) * xScale * 0.5f, -(i_top + i_bottom) * yScale * 0.5f, (i_z_nearPlane + i_z_farPlane) * zScale * 0.5f, 1.0f);
#endif
}

// Initialize / Clean Up
//----------------------

eae6320::Math::cMatrix_transformation::cMatrix_transformation( const cQuaternion& i_rotation, const sVector& i_translation )
	:
	m_30( 0.0f ), m_31( 0.0f ), m_32( 0.0f ),
	m_03( i_translation.x ), m_13( i_translation.y ), m_23( i_translation.z ),
	m_33( 1.0f )
{
	const auto _2x = i_rotation.m_x + i_rotation.m_x;
	const auto _2y = i_rotation.m_y + i_rotation.m_y;
	const auto _2z = i_rotation.m_z + i_rotation.m_z;
	const auto _2xx = i_rotation.m_x * _2x;
	const auto _2xy = _2x * i_rotation.m_y;
	const auto _2xz = _2x * i_rotation.m_z;
	const auto _2xw = _2x * i_rotation.m_w;
	const auto _2yy = _2y * i_rotation.m_y;
	const auto _2yz = _2y * i_rotation.m_z;
	const auto _2yw = _2y * i_rotation.m_w;
	const auto _2zz = _2z * i_rotation.m_z;
	const auto _2zw = _2z * i_rotation.m_w;

	m_00 = 1.0f - _2yy - _2zz;
	m_01 = _2xy - _2zw;
	m_02 = _2xz + _2yw;

	m_10 = _2xy + _2zw;
	m_11 = 1.0f - _2xx - _2zz;
	m_12 = _2yz - _2xw;

	m_20 = _2xz - _2yw;
	m_21 = _2yz + _2xw;
	m_22 = 1.0f - _2xx - _2yy;
}
