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
#include <Engine/Assets/ReferenceCountedAssets.h>

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
			// This defines the actual reference counting functions
			// This must be public
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

			void Draw() const;

			// factory function
			static cResult CreateMesh(cMesh*& o_mesh, const void* i_vertexData,    const uint16_t i_vertexCount,
				                                           const uint16_t* i_indexData, const uint16_t i_indexCount);

		private:
			// This prevents the class or struct from using illegal functions
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cMesh)

			// This declares the reference count member variable
			EAE6320_ASSETS_DECLAREREFERENCECOUNT()

			uint16_t m_indexCount = 0;
			uint16_t m_indexOfFirstVertexToRender = 0;

#if defined( EAE6320_PLATFORM_D3D )

			eae6320::Graphics::cVertexFormat* m_vertexFormat = nullptr;
			ID3D11Buffer* m_vertexBuffer = nullptr;
			ID3D11Buffer* m_indexBuffer = nullptr;

#elif defined( EAE6320_PLATFORM_GL )

			GLuint m_vertexArrayId = 0;
			GLuint m_vertexBufferId = 0;
			GLuint m_indexBufferId = 0;

#endif

			cMesh() = default;
			~cMesh();

			cResult Initialize(const void* i_vertexData,    const uint16_t i_vertexCount,
				               const uint16_t* i_indexData, const uint16_t i_indexCount);

			cResult CleanUp();


		};
	}
}


#endif	// EAE6320_GRAPHICS_CMESH_H
