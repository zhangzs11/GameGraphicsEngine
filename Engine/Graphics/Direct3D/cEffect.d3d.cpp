// Includes
//=========

#include "Includes.h"
#include "../cEffect.h"
#include "../cShader.h"
#include "../sContext.h"
#include "../cRenderState.h"

#include <Engine/Asserts/Asserts.h>

eae6320::cResult eae6320::Graphics::cEffect::Initialize(const char* i_vertexShaderPath, const char* i_fragmentShaderPath, 
	                                                    const uint8_t i_renderStateBits, const char* const i_texturePath)
{
	auto result = eae6320::Results::Success;

	// Load Vertex Shader
	{
		if (!(result = cShader::Load(i_vertexShaderPath, m_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
		{
			EAE6320_ASSERTF(false, "Can't initialize effect without vertex shader");
			return result;
		}
	}

	// Load Fragment Shader
	{
		if (!(result = cShader::Load(i_fragmentShaderPath, m_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
		{
			EAE6320_ASSERTF(false, "Can't initialize effect without pixel shader");
			return result;
		}
	}

	// Initialize Render State
	if (!(result = m_renderState.Initialize(i_renderStateBits)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
		return result;
	}

	// Load Texture
	{
		if (!(result = cTexture::CreateTextureDDS(m_texture, i_texturePath)))
		{
			EAE6320_ASSERTF(false, "Can't initialize effect without texture");
			return result;
		}
		// cTexture::CreateTextureWIC(m_texture, i_texturePath);
	}

	// Load Sampler
	{
		if (!(result = cSampler::CreateSampler(m_sampler)))
		{
			EAE6320_ASSERTF(false, "Can't initialize effect without sampler");
			return result;
		}
	}

	return result;
}

void eae6320::Graphics::cEffect::Bind() const
{
	auto* const direct3dImmediateContext = eae6320::Graphics::sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext != nullptr);

	// Bind Vertex Shader
	EAE6320_ASSERT((m_vertexShader != nullptr) && (m_vertexShader->m_shaderObject.vertex != nullptr));
	constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
	constexpr unsigned int interfaceCount = 0;
	direct3dImmediateContext->VSSetShader(m_vertexShader->m_shaderObject.vertex, noInterfaces, interfaceCount);

	// Bind Pixel Shader
	EAE6320_ASSERT((m_fragmentShader != nullptr) && (m_fragmentShader->m_shaderObject.fragment != nullptr));
	direct3dImmediateContext->PSSetShader(m_fragmentShader->m_shaderObject.fragment, noInterfaces, interfaceCount);

	// Bind Render State
	m_renderState.Bind();

	// Bind Texture
	if (m_texture) {
		m_texture->Bind(0);
	}

	// Bind Sampler
	if (m_sampler) {
		m_sampler->Bind(0);
	}

}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = eae6320::Results::Success;

	// Release the Vertex Shader
	if (m_vertexShader)
	{
		m_vertexShader->DecrementReferenceCount();
		m_vertexShader = nullptr;
	}

	// Release the Pixel Shader
	if (m_fragmentShader)
	{
		m_fragmentShader->DecrementReferenceCount();
		m_fragmentShader = nullptr;
	}

	// Release the Texture
	if (m_texture)
	{
		m_texture->DecrementReferenceCount();
		m_texture = nullptr;
	}
	// Clean up Render State
	// TODO :
	// Clean up the Sampler
	return result;
}