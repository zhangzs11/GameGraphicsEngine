#ifndef EAE6320_GRAPHICS_CEFFECT_H
#define EAE6320_GRAPHICS_CEFFECT_H

// Includes
//=========

#include "Configuration.h"
#include "cRenderState.h"
#include "cTexture.h"
#include "cSampler.h"

#ifdef EAE6320_PLATFORM_GL
#include "OpenGL/Includes.h"
#endif

#include <cstdint>
#include <Engine/Results/Results.h>
#include <Engine/Assets/ReferenceCountedAssets.h>

// Forward Declarations
//=====================

namespace eae6320
{
    namespace Graphics
    {
        class cShader;
    }
}

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cEffect
		{
        public:
            // This defines the actual reference counting functions
            // This must be public
            EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

            // Bind the effect (shaders and render state)
            void Bind() const;

            // factory function
            static cResult CreateEffect(cEffect*& o_effect, const char* i_vertexShaderPath, const char* i_fragmentShaderPath, 
                                                            const uint8_t i_renderStateBits, const char* const i_texturePath);

        private:
            // This prevents the class or struct from using illegal functions
            EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEffect)

            // This declares the reference count member variable
            EAE6320_ASSETS_DECLAREREFERENCECOUNT()

            eae6320::Graphics::cRenderState m_renderState;
            eae6320::Graphics::cTexture* m_texture = nullptr;
            eae6320::Graphics::cSampler* m_sampler = nullptr;

#if defined( EAE6320_PLATFORM_D3D )
            // Direct3D specific members
            eae6320::Graphics::cShader* m_vertexShader = nullptr;
            eae6320::Graphics::cShader* m_fragmentShader = nullptr;
#elif defined( EAE6320_PLATFORM_GL )
            // OpenGL specific members
            GLuint m_programId = 0;
#endif

            cEffect() = default;
            ~cEffect();

            // Initialize the effect with shader paths
            cResult Initialize(const char* i_vertexShaderPath, const char* i_fragmentShaderPath, 
                               const uint8_t i_renderStateBits, const char* const i_texturePath);
            // Clean up the effect
            cResult CleanUp();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CEFFECT_H
