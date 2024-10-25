// Includes
//=========

#include "../cTexture.h"

#include "Includes.h"
#include "../sContext.h"
#include "../External/WICTextureLoader11.h"
#include "../External/DDSTextureLoader11.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

eae6320::cResult eae6320::Graphics::cTexture::InitializeWIC(const char* const i_path)
{
	auto result = Results::Success;

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);

	//char* transform into wchar_t*
	wchar_t wPath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, i_path, -1, wPath, MAX_PATH);

	ID3D11Resource* textureResource = nullptr;
	const auto d3dResult = DirectX::CreateWICTextureFromFile(direct3dDevice, wPath, &textureResource, &m_textureView);

	if (FAILED(d3dResult))
	{
		result = Results::Failure;
		EAE6320_ASSERTF(false, "Failed to load WIC texture from file: %s", i_path);
		eae6320::Logging::OutputError("Direct3D failed to load WIC texture from file (HRESULT %#010x)", d3dResult);
		return result;
	}

	if (textureResource)
	{
		textureResource->Release();
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cTexture::InitializeDDS(const char* const i_path)
{
	auto result = Results::Success;

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);

	//char* transform into wchar_t*
	wchar_t wPath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, i_path, -1, wPath, MAX_PATH);

	ID3D11Resource* textureResource = nullptr;
	const auto d3dResult = DirectX::CreateDDSTextureFromFile(direct3dDevice, wPath, &textureResource, &m_textureView);

	if (FAILED(d3dResult))
	{
		result = Results::Failure;
		EAE6320_ASSERTF(false, "Failed to load DDS texture from file: %s", i_path);
		eae6320::Logging::OutputError("Direct3D failed to load DDS texture from file (HRESULT %#010x)", d3dResult);
		return result;
	}

	if (textureResource)
	{
		textureResource->Release();
	}

	return result;
}

void eae6320::Graphics::cTexture::Bind(unsigned int i_textureSlot) const
{
	auto* const direct3dImmediateContext = eae6320::Graphics::sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	EAE6320_ASSERT(i_textureSlot < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	direct3dImmediateContext->PSSetShaderResources(i_textureSlot, 1, &m_textureView);
}

eae6320::cResult eae6320::Graphics::cTexture::CleanUp()
{
	auto result = Results::Success;

	// Release Shader Resource View
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = nullptr;
	}

	return result;
}