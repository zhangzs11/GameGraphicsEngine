#ifndef EAE6320_GRAPHICS_CVIEW_H
#define EAE6320_GRAPHICS_CVIEW_H

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
		struct sInitializationParameters;
	}
}

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cView
		{
		public:
			cResult Initialize(const sInitializationParameters& i_initializationParameters);
			void Clear(const float i_clearColor[4]);
			void Bind();
			cResult CleanUp();

		private:
#if defined( EAE6320_PLATFORM_D3D )
			ID3D11RenderTargetView* m_renderTargetView = nullptr;
			ID3D11DepthStencilView* m_depthStencilView = nullptr;
#endif
		};
	}
}

#endif // EAE6320_GRAPHICS_CVIEW_H
