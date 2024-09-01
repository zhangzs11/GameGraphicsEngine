#ifndef EAE6320_GRAPHICS_CMESH_H
#define EAE6320_GRAPHICS_CMESH_H

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
		class cVertexFormat;
	}
}

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11Buffer;
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cMesh
		{
		public:
			cResult Initialize(const void* i_vertexData, const uint16_t i_vertexCount);

			void Draw() const;

			cResult CleanUp();

		private:
			unsigned int m_vertexCount = 0;
			unsigned int m_indexOfFirstVertexToRender = 0;

#if defined( EAE6320_PLATFORM_D3D )

			eae6320::Graphics::cVertexFormat* m_vertexFormat = nullptr;
			ID3D11Buffer* m_vertexBuffer = nullptr;

#elif defined( EAE6320_PLATFORM_GL )

			GLuint m_vertexArrayId = 0;
			GLuint m_vertexBufferId = 0;

#endif
		};
	}
}


#endif	// EAE6320_GRAPHICS_CMESH_H
