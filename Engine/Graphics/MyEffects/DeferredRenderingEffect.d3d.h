#ifndef EAE6320_GRAPHICS_DEFERRED_RENDERING_EFFECT_H
#define EAE6320_GRAPHICS_DEFERRED_RENDERING_EFFECT_H

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
        class cView_StructuredBuffer;
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
        // One Mesh----One Effect 
        class DeferredRenderingEffect_Geometry
        {
        public:
            EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

            // Bind the effect
            void Bind() const;

            // factory function
            static cResult CreateDeferredRenderingEffect_Geometry
            (
                DeferredRenderingEffect_Geometry*& o_effect,
                const char* i_vertexShaderPath,
                const char* i_fragmentShaderPath,
                const uint8_t i_renderStateBits,
                const std::string& texture0Path,
                const eSamplerType texture_samplerType
            );

        private:
            // This prevents the class or struct from using illegal functions
            EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(DeferredRenderingEffect_Geometry)

            // This declares the reference count member variable
            EAE6320_ASSETS_DECLAREREFERENCECOUNT()

            DeferredRenderingEffect_Geometry() = default;
            ~DeferredRenderingEffect_Geometry();

            // Shader
            eae6320::Graphics::cShader* m_vertexShader     = nullptr;
            eae6320::Graphics::cShader* m_fragmentShader   = nullptr;

            // Render State
            eae6320::Graphics::cRenderState m_renderState;

            // Texture
            eae6320::Graphics::cTexture* m_texture0        = nullptr;  // t0
            
            // Sampler
            eae6320::Graphics::cSampler* m_sampler_texture = nullptr;  // s0

            // Initialize the effect
            cResult Initialize
            (
                const char* i_vertexShaderPath,
                const char* i_fragmentShaderPath,
                const uint8_t i_renderStateBits,
                const std::string& texture0Path,
                const eSamplerType texture_samplerType
            );

            // Clean up the effect
            cResult CleanUp();
        };


        // Only Need One Lighting Effect (Only One Set of GBuffer)
        class DeferredRenderingEffect_Lighting
        {
        public:
            EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

            // Bind the effect
            void Bind() const;

            // factory function
            static cResult CreateDeferredRenderingEffect_Lighting
            (
                DeferredRenderingEffect_Lighting*& o_effect,
                const char* i_vertexShaderPath,
                const char* i_fragmentShaderPath,
                const uint8_t i_renderStateBits
            );

            // Set SRV
            void SetGBufferSRV(cView_SRV* i_GBuffer_01, 
                              cView_SRV* i_GBuffer_02, 
                              cView_SRV* i_GBuffer_03, 
                              cView_SRV* i_depthTexture) 
            {
                m_GBuffer_01 = i_GBuffer_01;
                m_GBuffer_02 = i_GBuffer_02;
                m_GBuffer_03 = i_GBuffer_03;
                m_depthTexture = i_depthTexture;
            }

            void SetStructedBuffer(cView_StructuredBuffer* i_structedBuffer)
            {
                m_structedBuffer_light = i_structedBuffer;
            }

        private:
            // This prevents the class or struct from using illegal functions
            EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(DeferredRenderingEffect_Lighting)

            // This declares the reference count member variable
            EAE6320_ASSETS_DECLAREREFERENCECOUNT()

            DeferredRenderingEffect_Lighting() = default;
            ~DeferredRenderingEffect_Lighting();

            // Shader
            eae6320::Graphics::cShader* m_vertexShader     = nullptr;
            eae6320::Graphics::cShader* m_fragmentShader   = nullptr;

            // Render State
            eae6320::Graphics::cRenderState m_renderState;

            // Shader Resource View (Set)
            cView_SRV* m_GBuffer_01                        = nullptr;  // t1
            cView_SRV* m_GBuffer_02                        = nullptr;  // t2
            cView_SRV* m_GBuffer_03                        = nullptr;  // t3
            cView_SRV* m_depthTexture                      = nullptr;  // t4

            // Structed Buffer (Set)
            cView_StructuredBuffer* m_structedBuffer_light = nullptr;  // t5

            // Initialize the effect
            cResult Initialize
            (
                const char* i_vertexShaderPath,
                const char* i_fragmentShaderPath,
                const uint8_t i_renderStateBits
            );

            // Clean up the effect
            cResult CleanUp();
        };
    }
}



#endif // EAE6320_GRAPHICS_DEFERRED_RENDERING_EFFECT_H

