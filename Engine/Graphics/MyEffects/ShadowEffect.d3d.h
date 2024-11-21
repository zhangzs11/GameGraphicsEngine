#ifndef EAE6320_GRAPHICS_SHADOW_EFFECT_H
#define EAE6320_GRAPHICS_SHADOW_EFFECT_H

#include "../Configuration.h"
#include "../cRenderState.h"

#include <Engine/Results/Results.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
// Forward Declarations
//=====================

namespace eae6320
{
    namespace Graphics
    {
        class cShader;
        class cTexture;
        class cSampler;
        class cRenderState;
        class cView_DSV;
        class cView_SRV;
        enum class eSamplerType;
    }
}

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
#endif

namespace eae6320
{
    namespace Graphics
    {
        class ShadowEffect
        {
        public:
            EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()
            // Bind the effect
            void Bind() const;

            // factory function
            static cResult CreateShadowEffect(
                ShadowEffect*& o_effect,
                const char* i_vertexShaderPath,
                const uint8_t i_renderStateBits
            );

        private:
            // This prevents the class or struct from using illegal functions
            EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(ShadowEffect)

            // This declares the reference count member variable
            EAE6320_ASSETS_DECLAREREFERENCECOUNT()

            ShadowEffect() = default;
            ~ShadowEffect();

            // Shader
            eae6320::Graphics::cShader* m_vertexShader = nullptr;

            // Render State
            eae6320::Graphics::cRenderState m_renderState;

            // Initialize the effect
            cResult Initialize(
                const char* i_vertexShaderPath,
                const uint8_t i_renderStateBits
            );

            // Clean up the effect
            cResult CleanUp();
        };
    }
}


#endif // EAE6320_GRAPHICS_SHADOW_EFFECT_H
