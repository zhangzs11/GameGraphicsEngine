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

		enum class eViewType : uint8_t
		{
			Screen,
			ShadowMap
		};
	}
}

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct D3D11_VIEWPORT;
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
			cResult Initialize(const sInitializationParameters& i_initializationParameters, eViewType i_viewType);
			void Clear(const float i_clearColor[4]);
			void Bind();
			cResult CleanUp();

			ID3D11ShaderResourceView* GetshaderResourceView() { return m_shaderResourceView; }
		private:
			eViewType m_viewType = eViewType::Screen;

#if defined( EAE6320_PLATFORM_D3D )
			D3D11_VIEWPORT* m_viewPort = nullptr;

			ID3D11RenderTargetView* m_renderTargetView = nullptr;     // For Screen
			ID3D11DepthStencilView* m_depthStencilView = nullptr;     // For Screen and shadow map
			ID3D11ShaderResourceView* m_shaderResourceView = nullptr; // For shadow map
#endif
		};
	}
}

#endif // EAE6320_GRAPHICS_CVIEW_H
