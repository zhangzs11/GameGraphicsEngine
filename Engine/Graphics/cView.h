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
#include <vector>

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
struct ID3D11Texture2D;
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		enum class eRenderTargetType
		{
			Screen,
			Texture
		};

		class cView_RTV
		{
		public:
			cResult Initialize(const sInitializationParameters& i_initializationParameters, 
				               eRenderTargetType i_targetType);
			void Clear(const float i_clearColor[4]);
			
			cResult CleanUp();
		public:
			D3D11_VIEWPORT* m_viewPort = nullptr;
			ID3D11RenderTargetView* m_renderTargetView = nullptr;
			ID3D11Texture2D* m_TextureBuffer = nullptr;
		};

		class cView_DSV
		{
		public:
			cResult Initialize(const sInitializationParameters& i_initializationParameters);
			void Clear(const float i_clearColor[4]);
			
			cResult CleanUp();
		public:
			D3D11_VIEWPORT* m_viewPort = nullptr;
			ID3D11DepthStencilView* m_depthStencilView = nullptr;
			ID3D11Texture2D* m_TextureBuffer = nullptr;
		};

		enum class BufferType
		{
			Depth,
			RenderTarget
		};

		class cView_SRV
		{
		public:
			cResult Initialize(const sInitializationParameters& i_initializationParameters,
				               ID3D11Texture2D* i_TextureBuffer,
				               BufferType i_bufferType);
			cResult CleanUp();
		public:
			D3D11_VIEWPORT* m_viewPort = nullptr;
			ID3D11ShaderResourceView* m_shaderResourceView = nullptr;

			ID3D11Texture2D* m_TextureBuffer = nullptr;
		};

		class cView_DSV_Array
		{
		public:
			cResult Initialize(const sInitializationParameters& i_initializationParameters, 
				               const uint32_t ArraySize);
			void Clear(const float i_clearColor[4]);
			cResult CleanUp();
		public:
			std::vector<ID3D11DepthStencilView*> m_depthStencilViewArray;
			std::vector<D3D11_VIEWPORT*> m_viewports;
			ID3D11Texture2D* m_TextureArrayBuffer = nullptr;
			uint32_t m_TextureArraySize = 0;
		};

		class cView_SRV_Array
		{
		public:
			eae6320::cResult Initialize(const sInitializationParameters& i_initializationParameters,
				                        ID3D11Texture2D* i_textureArray, 
				                        uint32_t arraySize, 
				                        BufferType i_bufferType);

			eae6320::cResult CleanUp();

		public:
			D3D11_VIEWPORT* m_viewPort = nullptr;
			ID3D11ShaderResourceView* m_shaderResourceView = nullptr;
			ID3D11Texture2D* m_textureArrayBuffer = nullptr;
			uint32_t m_arraySize = 0;
		};
	}
}

#endif // EAE6320_GRAPHICS_CVIEW_H
