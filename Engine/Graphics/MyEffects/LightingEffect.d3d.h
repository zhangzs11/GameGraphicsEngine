#ifndef EAE6320_GRAPHICS_LIGHT_EFFECT_H
#define EAE6320_GRAPHICS_LIGHT_EFFECT_H

#include "../Configuration.h"
#include "../cRenderState.h"

#include <string>

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
        class cView_SRV;
        enum class eSamplerType;
    }
}

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
#endif

namespace eae6320
{
    namespace Graphics
    {
        class LightingEffect
        {
        public:
            EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()
            // Bind the effect
            void Bind() const;

            // factory function
            static cResult CreateLightingEffect(
                LightingEffect*& o_effect,
                const char* i_vertexShaderPath, 
                const char* i_fragmentShaderPath,
                const uint8_t i_renderStateBits,
                const std::string& texture0Path,
                const std::string& texture1Path,
                const eSamplerType texture_samplerType,
                const eSamplerType shadowmap_samplerType
                );

            void SetShadowMapSRV(cView_SRV* i_shadowMap) { m_shadowMap = i_shadowMap; }

        private:
            // This prevents the class or struct from using illegal functions
            EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(LightingEffect)

            // This declares the reference count member variable
            EAE6320_ASSETS_DECLAREREFERENCECOUNT()

            LightingEffect() = default;
            ~LightingEffect();

            // Shader
            eae6320::Graphics::cShader* m_vertexShader = nullptr;
            eae6320::Graphics::cShader* m_fragmentShader = nullptr;

            // Render State
            eae6320::Graphics::cRenderState m_renderState;

            // Texture
            eae6320::Graphics::cTexture* m_texture0 = nullptr;                   // t0
            eae6320::Graphics::cTexture* m_texture1 = nullptr;                   // t1

            // Shader Resource View (Set)
            cView_SRV* m_shadowMap = nullptr;                                    // t2

            // Sampler
            eae6320::Graphics::cSampler* m_sampler_texture = nullptr;            // s0
            eae6320::Graphics::cSampler* m_compare_sampler_shadowMap = nullptr;  // s1

            // Initialize the effect
            cResult Initialize(
                const char* i_vertexShaderPath,
                const char* i_fragmentShaderPath,
                const uint8_t i_renderStateBits,
                const std::string& texture0Path,
                const std::string& texture1Path,
                const eSamplerType texture_samplerType,
                const eSamplerType shadowmap_samplerType
            );

            // Clean up the effect
            cResult CleanUp();
        };
    }
}


#endif // EAE6320_GRAPHICS_LIGHT_EFFECT_H
