// Includes
//=========

#include "Includes.h"
#include "../cView.h"
#include "../sContext.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

eae6320::cResult eae6320::Graphics::cView_RTV::Initialize(const sInitializationParameters& i_initializationParameters,
	                                                      eRenderTargetType i_targetType)
{
	auto result = eae6320::Results::Success;

	ID3D11Texture2D* targetTexture = nullptr;
	eae6320::cScopeGuard scopeGuard([&targetTexture]
		{
			// Regardless of success or failure the two texture resources should be released
			// (if the function is successful the views will hold internal references to the resources)
			if (targetTexture)
			{
				targetTexture->Release();
				targetTexture = nullptr;
			}
		});

	auto& g_context = eae6320::Graphics::sContext::g_context;
	auto* const direct3dDevice = g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);
	// auto* const direct3dImmediateContext = g_context.direct3dImmediateContext;
	// EAE6320_ASSERT(direct3dImmediateContext);

	// Get the back buffer from the swap chain
	if (i_targetType == eRenderTargetType::Screen)
	{
		constexpr unsigned int bufferIndex = 0;	// This must be 0 since the swap chain is discarded
		const auto d3dResult = g_context.swapChain->GetBuffer(bufferIndex, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&targetTexture));
		if (FAILED(d3dResult))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "Couldn't get the back buffer from the swap chain (HRESULT %#010x)", d3dResult);
			eae6320::Logging::OutputError("Direct3D failed to get the back buffer from the swap chain (HRESULT %#010x)", d3dResult);
			return result;
		}
	}
	else if (i_targetType == eRenderTargetType::Texture)
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = i_initializationParameters.resolutionWidth;
		textureDesc.Height = i_initializationParameters.resolutionHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		const auto d3dResult = direct3dDevice->CreateTexture2D(&textureDesc, nullptr, &targetTexture);
		if (FAILED(d3dResult))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "Couldn't create render target texture (HRESULT %#010x)", d3dResult);
			eae6320::Logging::OutputError("Direct3D failed to create render target texture (HRESULT %#010x)", d3dResult);
			return result;
		}
	}
	// Create the view
	{
		constexpr D3D11_RENDER_TARGET_VIEW_DESC* const accessAllSubResources = nullptr;
		const auto d3dResult = direct3dDevice->CreateRenderTargetView(targetTexture, accessAllSubResources, &m_renderTargetView);
		if (FAILED(d3dResult))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "Couldn't create render target view (HRESULT %#010x)", d3dResult);
			eae6320::Logging::OutputError("Direct3D failed to create the render target view (HRESULT %#010x)", d3dResult);
			return result;
		}

		m_TextureBuffer = targetTexture;
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

eae6320::cResult eae6320::Graphics::cView_DSV_Array::Initialize(const sInitializationParameters& i_initializationParameters,
	                                                            const uint32_t ArraySize)
{
	auto result = eae6320::Results::Success;

	ID3D11Texture2D* depthBufferArray = nullptr;
	eae6320::cScopeGuard scopeGuard([&depthBufferArray]
		{
			if (depthBufferArray)
			{
				depthBufferArray->Release();
				depthBufferArray = nullptr;
			}
		});

	auto& g_context = eae6320::Graphics::sContext::g_context;
	auto* const direct3dDevice = g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);

	// Create the Texture2D Array
	{
		D3D11_TEXTURE2D_DESC textureDescription = {};
		textureDescription.Width = i_initializationParameters.resolutionWidth;
		textureDescription.Height = i_initializationParameters.resolutionHeight;
		textureDescription.MipLevels = 1;
		textureDescription.ArraySize = ArraySize; // Number of cascades
		textureDescription.Format = DXGI_FORMAT_R32_TYPELESS;
		textureDescription.SampleDesc.Count = 1;
		textureDescription.Usage = D3D11_USAGE_DEFAULT;
		textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		textureDescription.CPUAccessFlags = 0;

		const auto d3dResult = direct3dDevice->CreateTexture2D(&textureDescription, nullptr, &depthBufferArray);
		if (FAILED(d3dResult))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "Couldn't create depth buffer array (HRESULT %#010x)", d3dResult);
			return result;
		}
		m_TextureArrayBuffer = depthBufferArray;
	}

	// Create DSVs and Viewports for each array slice
	for (uint32_t i = 0; i < ArraySize; ++i)
	{
		// Create DSV
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Texture2DArray.FirstArraySlice = i;
		dsvDesc.Texture2DArray.ArraySize = 1;

		ID3D11DepthStencilView* dsv = nullptr;
		const auto d3dResult = direct3dDevice->CreateDepthStencilView(m_TextureArrayBuffer, &dsvDesc, &dsv);
		if (FAILED(d3dResult))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "Couldn't create depth stencil view for slice %u (HRESULT %#010x)", i, d3dResult);
			return result;
		}
		m_depthStencilViewArray.push_back(dsv);

		// Create Viewport
		D3D11_VIEWPORT* viewport = new D3D11_VIEWPORT();
		viewport->TopLeftX = 0.0f;
		viewport->TopLeftY = 0.0f;
		viewport->Width = static_cast<float>(i_initializationParameters.resolutionWidth);
		viewport->Height = static_cast<float>(i_initializationParameters.resolutionHeight);
		viewport->MinDepth = 0.0f;
		viewport->MaxDepth = 1.0f;

		m_viewports.push_back(viewport);
	}

	m_TextureArraySize = ArraySize;

	return result;
}

eae6320::cResult eae6320::Graphics::cView_SRV::Initialize(const sInitializationParameters& i_initializationParameters, 
	ID3D11Texture2D* i_TextureBuffer,
	BufferType i_bufferType)
{
	auto result = eae6320::Results::Success;

	auto& g_context = eae6320::Graphics::sContext::g_context;
	auto* const direct3dDevice = g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);
	auto* const direct3dImmediateContext = g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	if (i_bufferType == BufferType::Depth)
	{
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	}
	else if (i_bufferType == BufferType::RenderTarget)
	{
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
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

eae6320::cResult eae6320::Graphics::cView_SRV_Array::Initialize(const sInitializationParameters& i_initializationParameters,
	                                                            ID3D11Texture2D* i_textureArray,
	                                                            uint32_t arraySize,
	                                                            BufferType i_bufferType)
{
	auto result = eae6320::Results::Success;

	auto& g_context = eae6320::Graphics::sContext::g_context;
	auto* const direct3dDevice = g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	if (i_bufferType == BufferType::Depth)
	{
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	}
	else if (i_bufferType == BufferType::RenderTarget)
	{
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1; 
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = arraySize;

	const auto d3dCreateSRVResult = direct3dDevice->CreateShaderResourceView(i_textureArray, &srvDesc, &m_shaderResourceView);
	if (FAILED(d3dCreateSRVResult))
	{
		return eae6320::Results::Failure;
	}

	m_textureArrayBuffer = i_textureArray;
	m_arraySize = arraySize;

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

// CLEAR
// -----------------------------------------------------

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

void eae6320::Graphics::cView_DSV_Array::Clear(const float i_clearColor[4])
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	constexpr float clearToFarDepth = 1.0f;
	constexpr uint8_t stencilValue = 0;

	for (auto* dsv : m_depthStencilViewArray) {
		direct3dImmediateContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, clearToFarDepth, stencilValue);
	}
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

	if (m_viewPort)
	{
		delete m_viewPort;
		m_viewPort = nullptr;
	}

	return Results::Success;
}

eae6320::cResult eae6320::Graphics::cView_DSV_Array::CleanUp()
{
	for (auto* dsv : m_depthStencilViewArray)
	{
		if (dsv)
		{
			dsv->Release();
			dsv = nullptr;
		}
	}
	m_depthStencilViewArray.clear();

	/*if (m_TextureArrayBuffer)
	{
		m_TextureArrayBuffer->Release();
		m_TextureArrayBuffer = nullptr;
	}*/

	for (auto* viewport : m_viewports) {
		if (viewport) {
			delete viewport;
			viewport = nullptr;
		}
	}

	return eae6320::Results::Success;

}

eae6320::cResult eae6320::Graphics::cView_SRV_Array::CleanUp()
{
	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = nullptr;
	}

	if (m_viewPort)
	{
		delete m_viewPort;
		m_viewPort = nullptr;
	}

	return eae6320::Results::Success;
}