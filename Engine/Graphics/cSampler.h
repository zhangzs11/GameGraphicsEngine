#ifndef EAE6320_GRAPHICS_CSAMPLER_H
#define EAE6320_GRAPHICS_CSAMPLER_H

#include <cstdint>
#include <Engine/Results/Results.h>
#include <Engine/Assets/ReferenceCountedAssets.h>

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11SamplerState;
#endif

// Enum Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		enum class eSamplerType
		{
			Point,
			Linear,
			FXAA_LinearClamp,
			Anisotropic,
			Comparison_less_equal,

		};
	}
}


// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cSampler
		{
		public:
			static cResult CreateSampler(cSampler*& o_sampler, eSamplerType i_type);

			void Bind(unsigned int i_slot) const;

		private:
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cSampler)

#ifdef EAE6320_PLATFORM_D3D
				ID3D11SamplerState* m_samplerState = nullptr;
#endif
			cSampler() = default;
			~cSampler();

			cResult Initialize(eSamplerType i_type);
			cResult CleanUp();
		};
	}
}

#endif    // EAE6320_GRAPHICS_CSAMPLER_H