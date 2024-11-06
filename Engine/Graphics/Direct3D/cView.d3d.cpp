// Includes
//=========

#include "Includes.h"
#include "../cView.h"
#include "../sContext.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

eae6320::cResult eae6320::Graphics::cView::Initialize(const sInitializationParameters& i_initializationParameters, eae6320::Graphics::eViewType i_viewType)
{
	auto result = eae6320::Results::Success;
	m_viewType = i_viewType;

	ID3D11Texture2D* backBuffer = nullptr;
	ID3D11Texture2D* depthBuffer = nullptr;
    eae6320::cScopeGuard scopeGuard([&backBuffer, &depthBuffer]
    {
        // Regardless of success or failure the two texture resources should be released
        // (if the function is successful the views will hold internal references to the resources)
        if (backBuffer)
        {
            backBuffer->Release();
            backBuffer = nullptr;
        }
        if (depthBuffer)
        {
            depthBuffer->Release();
            depthBuffer = nullptr;
        }
    });

    auto& g_context = eae6320::Graphics::sContext::g_context;
    auto* const direct3dDevice = g_context.direct3dDevice;
    EAE6320_ASSERT(direct3dDevice);
    auto* const direct3dImmediateContext = g_context.direct3dImmediateContext;
    EAE6320_ASSERT(direct3dImmediateContext);

	// Create a "render target view" of the back buffer
	// (the back buffer was already created by the call to D3D11CreateDeviceAndSwapChain(),
	// but a "view" of it is required to use as a "render target",
	// meaning a texture that the GPU can render to)
	if (m_viewType == eViewType::Screen)
	{
		// Get the back buffer from the swap chain
		{
			constexpr unsigned int bufferIndex = 0;	// This must be 0 since the swap chain is discarded
			const auto d3dResult = g_context.swapChain->GetBuffer(bufferIndex, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
			if (FAILED(d3dResult))
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, "Couldn't get the back buffer from the swap chain (HRESULT %#010x)", d3dResult);
				eae6320::Logging::OutputError("Direct3D failed to get the back buffer from the swap chain (HRESULT %#010x)", d3dResult);
				return result;
			}
		}
		// Create the view
		{
			constexpr D3D11_RENDER_TARGET_VIEW_DESC* const accessAllSubResources = nullptr;
			const auto d3dResult = direct3dDevice->CreateRenderTargetView(backBuffer, accessAllSubResources, &m_renderTargetView);
			if (FAILED(d3dResult))
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, "Couldn't create render target view (HRESULT %#010x)", d3dResult);
				eae6320::Logging::OutputError("Direct3D failed to create the render target view (HRESULT %#010x)", d3dResult);
				return result;
			}
		}
	}
	// Create a depth/stencil buffer and a view of it
	{
		// Unlike the back buffer no depth/stencil buffer exists until and unless it is explicitly created
		{

			D3D11_TEXTURE2D_DESC textureDescription{};

			textureDescription.MipLevels = 1;	// A depth buffer has no MIP maps
			textureDescription.ArraySize = 1;
			// textureDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 24 bits for depth and 8 bits for stencil
			textureDescription.Format = DXGI_FORMAT_R32_TYPELESS;
			{
				DXGI_SAMPLE_DESC& sampleDescription = textureDescription.SampleDesc;

				sampleDescription.Count = 1;	// No multisampling
				sampleDescription.Quality = 0;	// Doesn't matter when Count is 1
			}
			textureDescription.Usage = D3D11_USAGE_DEFAULT;	// Allows the GPU to write to it
			textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			textureDescription.CPUAccessFlags = 0;	// The CPU doesn't need access
			textureDescription.MiscFlags = 0;

			textureDescription.Width = i_initializationParameters.resolutionWidth;
			textureDescription.Height = i_initializationParameters.resolutionHeight;

			// The GPU renders to the depth/stencil buffer and so there is no initial data
			// (like there would be with a traditional texture loaded from disk)
			constexpr D3D11_SUBRESOURCE_DATA* const noInitialData = nullptr;
			const auto d3dResult = direct3dDevice->CreateTexture2D(&textureDescription, noInitialData, &depthBuffer);
			if (FAILED(d3dResult))
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, "Couldn't create depth buffer (HRESULT %#010x)", d3dResult);
				eae6320::Logging::OutputError("Direct3D failed to create the depth buffer resource (HRESULT %#010x)", d3dResult);
				return result;
			}
		}
		// Create DepthStencilView
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Set up the depth view format match DXGI_FORMAT_R32_TYPELESS
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			// constexpr D3D11_DEPTH_STENCIL_VIEW_DESC* const noSubResources = nullptr;
			const auto d3dResult = direct3dDevice->CreateDepthStencilView(depthBuffer, &dsvDesc, &m_depthStencilView);
			if (FAILED(d3dResult))
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, "Couldn't create depth stencil view (HRESULT %#010x)", d3dResult);
				eae6320::Logging::OutputError("Direct3D failed to create the depth stencil view (HRESULT %#010x)", d3dResult);
				return result;
			}
		}
	}

	if (m_viewType == eViewType::ShadowMap)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		const auto d3dCreateSRVResult = direct3dDevice->CreateShaderResourceView(depthBuffer, &srvDesc, &m_shaderResourceView);
		if (FAILED(d3dCreateSRVResult))
		{
			return eae6320::Results::Failure;
		}
	}

	// Specify that the entire render target should be visible
	{
		m_viewPort = new D3D11_VIEWPORT();
		m_viewPort->TopLeftX = m_viewPort->TopLeftY = 0.0f;
		m_viewPort->MinDepth = 0.0f;
		m_viewPort->MaxDepth = 1.0f;

		m_viewPort->Width = static_cast<float>(i_initializationParameters.resolutionWidth);
		m_viewPort->Height = static_cast<float>(i_initializationParameters.resolutionHeight);

	}
    return result;
}

eae6320::cResult eae6320::Graphics::cView_RTV::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = eae6320::Results::Success;

	ID3D11Texture2D* backBuffer = nullptr;
	eae6320::cScopeGuard scopeGuard([&backBuffer]
		{
			// Regardless of success or failure the two texture resources should be released
			// (if the function is successful the views will hold internal references to the resources)
			if (backBuffer)
			{
				backBuffer->Release();
				backBuffer = nullptr;
			}
		});

	auto& g_context = eae6320::Graphics::sContext::g_context;
	auto* const direct3dDevice = g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);
	auto* const direct3dImmediateContext = g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	// Get the back buffer from the swap chain
	{
		constexpr unsigned int bufferIndex = 0;	// This must be 0 since the swap chain is discarded
		const auto d3dResult = g_context.swapChain->GetBuffer(bufferIndex, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
		if (FAILED(d3dResult))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "Couldn't get the back buffer from the swap chain (HRESULT %#010x)", d3dResult);
			eae6320::Logging::OutputError("Direct3D failed to get the back buffer from the swap chain (HRESULT %#010x)", d3dResult);
			return result;
		}
	}
	// Create the view
	{
		constexpr D3D11_RENDER_TARGET_VIEW_DESC* const accessAllSubResources = nullptr;
		const auto d3dResult = direct3dDevice->CreateRenderTargetView(backBuffer, accessAllSubResources, &m_renderTargetView);
		if (FAILED(d3dResult))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "Couldn't create render target view (HRESULT %#010x)", d3dResult);
			eae6320::Logging::OutputError("Direct3D failed to create the render target view (HRESULT %#010x)", d3dResult);
			return result;
		}

		m_TextureBuffer = backBuffer;
	}

	// Specify that the entire render target should be visible
	{
		m_viewPort = new D3D11_VIEWPORT();
		m_viewPort->TopLeftX = m_viewPort->TopLeftY = 0.0f;
		m_viewPort->MinDepth = 0.0f;
		m_viewPort->MaxDepth = 1.0f;

		m_viewPort->Width = static_cast<float>(i_initializationParameters.resolutionWidth);
		m_viewPort->Height = static_cast<float>(i_initializationParameters.resolutionHeight);

	}

	return result;

}

eae6320::cResult eae6320::Graphics::cView_DSV::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = eae6320::Results::Success;

	ID3D11Texture2D* depthBuffer = nullptr;
	eae6320::cScopeGuard scopeGuard([&depthBuffer]
		{
			if (depthBuffer)
			{
				depthBuffer->Release();
				depthBuffer = nullptr;
			}
		});

	auto& g_context = eae6320::Graphics::sContext::g_context;
	auto* const direct3dDevice = g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);
	auto* const direct3dImmediateContext = g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);
	// Create a depth/stencil buffer and a view of it
	{
		// Unlike the back buffer no depth/stencil buffer exists until and unless it is explicitly created
		{

			D3D11_TEXTURE2D_DESC textureDescription{};

			textureDescription.MipLevels = 1;	// A depth buffer has no MIP maps
			textureDescription.ArraySize = 1;
			// textureDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 24 bits for depth and 8 bits for stencil
			textureDescription.Format = DXGI_FORMAT_R32_TYPELESS;
			{
				DXGI_SAMPLE_DESC& sampleDescription = textureDescription.SampleDesc;

				sampleDescription.Count = 1;	// No multisampling
				sampleDescription.Quality = 0;	// Doesn't matter when Count is 1
			}
			textureDescription.Usage = D3D11_USAGE_DEFAULT;	// Allows the GPU to write to it
			textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			textureDescription.CPUAccessFlags = 0;	// The CPU doesn't need access
			textureDescription.MiscFlags = 0;

			textureDescription.Width = i_initializationParameters.resolutionWidth;
			textureDescription.Height = i_initializationParameters.resolutionHeight;

			// The GPU renders to the depth/stencil buffer and so there is no initial data
			// (like there would be with a traditional texture loaded from disk)
			constexpr D3D11_SUBRESOURCE_DATA* const noInitialData = nullptr;
			const auto d3dResult = direct3dDevice->CreateTexture2D(&textureDescription, noInitialData, &depthBuffer);
			if (FAILED(d3dResult))
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, "Couldn't create depth buffer (HRESULT %#010x)", d3dResult);
				eae6320::Logging::OutputError("Direct3D failed to create the depth buffer resource (HRESULT %#010x)", d3dResult);
				return result;
			}
		}
		// Create DepthStencilView
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // Set up the depth view format match DXGI_FORMAT_R32_TYPELESS
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			// constexpr D3D11_DEPTH_STENCIL_VIEW_DESC* const noSubResources = nullptr;
			const auto d3dResult = direct3dDevice->CreateDepthStencilView(depthBuffer, &dsvDesc, &m_depthStencilView);
			if (FAILED(d3dResult))
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, "Couldn't create depth stencil view (HRESULT %#010x)", d3dResult);
				eae6320::Logging::OutputError("Direct3D failed to create the depth stencil view (HRESULT %#010x)", d3dResult);
				return result;
			}

			m_TextureBuffer = depthBuffer;
		}
	}

	// Specify that the entire render target should be visible
	{
		m_viewPort = new D3D11_VIEWPORT();
		m_viewPort->TopLeftX = m_viewPort->TopLeftY = 0.0f;
		m_viewPort->MinDepth = 0.0f;
		m_viewPort->MaxDepth = 1.0f;

		m_viewPort->Width = static_cast<float>(i_initializationParameters.resolutionWidth);
		m_viewPort->Height = static_cast<float>(i_initializationParameters.resolutionHeight);

	}

	return result;
}

eae6320::cResult eae6320::Graphics::cView_SRV::Initialize(const sInitializationParameters& i_initializationParameters, ID3D11Texture2D* i_TextureBuffer)
{
	auto result = eae6320::Results::Success;

	auto& g_context = eae6320::Graphics::sContext::g_context;
	auto* const direct3dDevice = g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);
	auto* const direct3dImmediateContext = g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	const auto d3dCreateSRVResult = direct3dDevice->CreateShaderResourceView(i_TextureBuffer, &srvDesc, &m_shaderResourceView);
	if (FAILED(d3dCreateSRVResult))
	{
		return eae6320::Results::Failure;
	}

	m_TextureBuffer = i_TextureBuffer;

	// Specify that the entire render target should be visible
	{
		m_viewPort = new D3D11_VIEWPORT();
		m_viewPort->TopLeftX = m_viewPort->TopLeftY = 0.0f;
		m_viewPort->MinDepth = 0.0f;
		m_viewPort->MaxDepth = 1.0f;

		m_viewPort->Width = static_cast<float>(i_initializationParameters.resolutionWidth);
		m_viewPort->Height = static_cast<float>(i_initializationParameters.resolutionHeight);

	}
	return result;
}

// CLEAR DATA
// -----------------------------------------------------
void eae6320::Graphics::cView::Clear(const float i_clearColor[4])
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	if (m_renderTargetView) {
		direct3dImmediateContext->ClearRenderTargetView(m_renderTargetView, i_clearColor);
	}
	// direct3dImmediateContext->ClearRenderTargetView(m_renderTargetView, i_clearColor);
	constexpr float clearToFarDepth = 1.0f;
	constexpr uint8_t stencilValue = 0;
	if (m_depthStencilView) {
		direct3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, clearToFarDepth, stencilValue);
	}
	// direct3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, clearToFarDepth, stencilValue);
}

void eae6320::Graphics::cView_RTV::Clear(const float i_clearColor[4])
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	if (m_renderTargetView) {
		direct3dImmediateContext->ClearRenderTargetView(m_renderTargetView, i_clearColor);
	}

}

void eae6320::Graphics::cView_DSV::Clear(const float i_clearColor[4])
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	constexpr float clearToFarDepth = 1.0f;
	constexpr uint8_t stencilValue = 0;
	if (m_depthStencilView) {
		direct3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, clearToFarDepth, stencilValue);
	}
}

void eae6320::Graphics::cView::Bind()
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	if (m_viewType == eViewType::Screen)
	{
		constexpr unsigned int renderTargetCount = 1;
		direct3dImmediateContext->OMSetRenderTargets(renderTargetCount, &m_renderTargetView, m_depthStencilView);
	}

	else if (m_viewType == eViewType::ShadowMap)
	{
		direct3dImmediateContext->OMSetRenderTargets(0, nullptr, m_depthStencilView);
	}

	constexpr unsigned int viewPortCount = 1;
	direct3dImmediateContext->RSSetViewports(viewPortCount, m_viewPort);
}

eae6320::cResult eae6320::Graphics::cView::CleanUp()
{
	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}
	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = nullptr;
	}
	delete m_viewPort;
	m_viewPort = nullptr;

	return Results::Success;
}


// CLEAN UP
// -----------------------------------------------------

eae6320::cResult eae6320::Graphics::cView_RTV::CleanUp()
{
	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}
	//if (m_TextureBuffer)
	//{
	//	m_TextureBuffer->Release();
	//	m_TextureBuffer = nullptr;
	//}
	if (m_viewPort)
	{
		delete m_viewPort;
		m_viewPort = nullptr;
	}

	return Results::Success;
}

eae6320::cResult eae6320::Graphics::cView_DSV::CleanUp()
{
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}
	//if (m_TextureBuffer)
	//{
	//	m_TextureBuffer->Release();
	//	m_TextureBuffer = nullptr;
	//}
	if (m_viewPort)
	{
		delete m_viewPort;
		m_viewPort = nullptr;
	}

	return Results::Success;
}

eae6320::cResult eae6320::Graphics::cView_SRV::CleanUp()
{
	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = nullptr;
	}
	//if (m_TextureBuffer)
	//{
	//	m_TextureBuffer->Release();
	//	m_TextureBuffer = nullptr;
	//}
	if (m_viewPort)
	{
		delete m_viewPort;
		m_viewPort = nullptr;
	}

	return Results::Success;
}