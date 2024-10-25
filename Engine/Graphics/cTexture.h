#ifndef EAE6320_GRAPHICS_CTEXTURE_H
#define EAE6320_GRAPHICS_CTEXTURE_H

// Includes
//=========

#include "Configuration.h"

#ifdef EAE6320_PLATFORM_GL
#include "OpenGL/Includes.h"
#endif

#include <cstdint>
#include <Engine/Results/Results.h>
#include <Engine/Assets/ReferenceCountedAssets.h>

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11ShaderResourceView;
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cTexture
		{
		public:
			// This defines the actual reference counting functions
			// This must be public
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

			void Bind(unsigned int i_textureSlot) const;

			// factory function
			static cResult CreateTextureWIC(cTexture*& o_texture, const char* const i_path);
			static cResult CreateTextureDDS(cTexture*& o_texture, const char* const i_path);

		private:
			// This prevents the class or struct from using illegal functions
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cTexture)

			// This declares the reference count member variable
			EAE6320_ASSETS_DECLAREREFERENCECOUNT()

#if defined(EAE6320_PLATFORM_D3D)
				// Direct3D specific texture resources
				ID3D11ShaderResourceView* m_textureView = nullptr;
#elif defined(EAE6320_PLATFORM_GL)
				// OpenGL specific texture resources
				GLuint m_textureId = 0;
#endif

			cTexture() = default;
			~cTexture();

			// Texture loading and initialization logic
			cResult InitializeWIC(const char* const i_path);
			cResult InitializeDDS(const char* const i_path);

			cResult CleanUp();
		};
	}
}


#endif	// EAE6320_GRAPHICS_CTEXTURE_H
