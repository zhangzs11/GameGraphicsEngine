#ifndef EAE6320_GRAPHICS_SLIGHT_H
#define EAE6320_GRAPHICS_SLIGHT_H

// Includes
//=========

#include <Engine/Math/sVector.h>
#include <Engine/Math/cQuaternion.h>
#include <Engine/Math/cMatrix_transformation.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		struct sDirectionalLight
		{
			sDirectionalLight() = default;

			sDirectionalLight(const eae6320::Math::sVector4& i_ambient, 
							  const eae6320::Math::sVector4& i_diffuse,
							  const eae6320::Math::sVector4& i_specular,
							  const eae6320::Math::sVector&  i_direction,
				              const eae6320::Math::sVector&  i_position) :
				ambient(i_ambient), diffuse(i_diffuse), specular(i_specular), direction(i_direction), pad(), 
			    position(i_position), pad2() {}

			// Data
			//=====

			eae6320::Math::sVector4 ambient;
			eae6320::Math::sVector4 diffuse;
			eae6320::Math::sVector4 specular;

			// 4 float
			eae6320::Math::sVector direction;
			float pad;
			eae6320::Math::sVector position; // only for the position of shadow map camera
			float pad2;
		};

		struct sPointLight
		{
			sPointLight() = default;

			sPointLight(const eae6320::Math::sVector4& i_ambient, 
				        const eae6320::Math::sVector4& i_diffuse,
				        const eae6320::Math::sVector4& i_specular,
				        const eae6320::Math::sVector& i_position,
				        float i_range, 
				        const eae6320::Math::sVector& i_att) :

				        ambient(i_ambient), diffuse(i_diffuse), specular(i_specular), position(i_position), 
				        range(i_range), att(i_att), pad() {}

			// Data
			//=====

			eae6320::Math::sVector4 ambient;
			eae6320::Math::sVector4 diffuse;
			eae6320::Math::sVector4 specular;

			// 4 float
			eae6320::Math::sVector position;
			float range;

			// 4 float
			eae6320::Math::sVector att;
			float pad;
		};

		struct sSpotLight
		{
			sSpotLight() = default;

			sSpotLight(const eae6320::Math::sVector4& i_ambient,
					   const eae6320::Math::sVector4& i_diffuse,
					   const eae6320::Math::sVector4& i_specular,
					   const eae6320::Math::sVector& i_position,
					   float i_range, 
					   const eae6320::Math::sVector& i_direction,
					   float i_spot, 
					   const eae6320::Math::sVector& i_att) :

					   ambient(i_ambient), diffuse(i_diffuse), specular(i_specular),
				       position(i_position), range(i_range), direction(i_direction), spot(i_spot), att(i_att), pad() {}

			// Data
			//=====

			eae6320::Math::sVector4 ambient;
			eae6320::Math::sVector4 diffuse;
			eae6320::Math::sVector4 specular;

			// 4 float
			eae6320::Math::sVector position;
			float range;

			// 4 float
			eae6320::Math::sVector direction;
			float spot;

			// 4 float
			eae6320::Math::sVector att;
			float pad;
		};

		struct sMaterial
		{
			sMaterial() = default;

			sMaterial(const eae6320::Math::sVector4& i_ambient,
					  const eae6320::Math::sVector4& i_diffuse,
					  const eae6320::Math::sVector4& i_specular,
					  const eae6320::Math::sVector4& i_reflect) :
					  ambient(i_ambient), diffuse(i_diffuse), specular(i_specular), reflect(i_reflect) {}

			// Data
			//=====

			eae6320::Math::sVector4 ambient;
			eae6320::Math::sVector4 diffuse;
			eae6320::Math::sVector4 specular;
			eae6320::Math::sVector4 reflect;
		};

		struct sPointLight_deferred
		{
			sPointLight_deferred() = default;

			sPointLight_deferred(const eae6320::Math::sVector& i_posV,
				float i_attenuationBegin,
				const eae6320::Math::sVector& i_color,
				float i_attenuationEnd) :
				posV(i_posV), attenuationBegin(i_attenuationBegin), color(i_color), attenuationEnd(i_attenuationEnd) {}

			// Data
			//=====

			eae6320::Math::sVector posV;
			float attenuationBegin;
			eae6320::Math::sVector color;
			float attenuationEnd;
		};

	}
}

#endif // EAE6320_GRAPHICS_SLIGHT_H
