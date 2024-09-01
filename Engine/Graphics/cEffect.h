#ifndef EAE6320_GRAPHICS_CEFFECT_H
#define EAE6320_GRAPHICS_CEFFECT_H

// Includes
//=========

#include "Configuration.h"

#ifdef EAE6320_PLATFORM_GL
#include "OpenGL/Includes.h"
#endif

#include <cstdint>
#include <Engine/Results/Results.h>

// Forward Declarations
//=====================

namespace eae6320
{
    namespace Graphics
    {
        class cRenderState;
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
            // Initialize the effect with shader paths
            cResult Initialize(const char* i_vertexShaderPath, const char* i_fragmentShaderPath);

            // Bind the effect (shaders and render state)
            void Bind() const;

            // Clean up the effect
            cResult CleanUp();

        private:
            // Direct3D specific members
            eae6320::Graphics::cShader* m_vertexShader = nullptr;
            eae6320::Graphics::cShader* m_fragmentShader = nullptr;
            eae6320::Graphics::cRenderState m_renderState;
#if defined( EAE6320_PLATFORM_GL )
            // OpenGL specific members
            GLuint m_programId = 0;
#endif
		};
	}
}

#endif	// EAE6320_GRAPHICS_CEFFECT_H
