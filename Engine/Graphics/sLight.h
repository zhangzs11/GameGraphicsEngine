#ifndef EAE6320_GRAPHICS_SLIGHT_H
#define EAE6320_GRAPHICS_SLIGHT_H

// Includes
//=========

#include <Engine/Math/sVector.h>

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
							  const eae6320::Math::sVector&  i_direction) :
				ambient(i_ambient), diffuse(i_diffuse), specular(i_specular), direction(i_direction), pad() {}
			

			// Data
			//=====

			eae6320::Math::sVector4 ambient;
			eae6320::Math::sVector4 diffuse;
			eae6320::Math::sVector4 specular;

			// 4 float
			eae6320::Math::sVector direction;
			float pad;
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

	}
}

#endif // EAE6320_GRAPHICS_SLIGHT_H
