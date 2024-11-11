// Includes
//=========
#include "../Direct3D/Includes.h"
#include "PostProcessingEffect.d3d.h"
#include "../cShader.h"
#include "../sContext.h"
#include "../cRenderState.h"
#include "../cView.h"
#include <new>

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::PostProcessingEffect::CreatePostProcessingEffect(
	PostProcessingEffect*& o_effect,
	const char* i_vertexShaderPath,
	const char* i_fragmentShaderPath,
	const uint8_t i_renderStateBits,
	const eSamplerType texture_samplerType
)
{
	auto result = eae6320::Results::Success;

	// allocate
	PostProcessingEffect* newEffect = new (std::nothrow) PostProcessingEffect();
	if (!newEffect)
	{
		result = eae6320::Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for effect");
		return result;
	}

	// initialize a new instance of class cEffect
	if (!(result = newEffect->Initialize(
		i_vertexShaderPath,
		i_fragmentShaderPath,
		i_renderStateBits,
		texture_samplerType)))
	{
		delete newEffect;
		newEffect = nullptr;
		return result;
	}

	o_effect = newEffect;

	return result;
}

eae6320::Graphics::PostProcessingEffect::~PostProcessingEffect()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}

eae6320::cResult eae6320::Graphics::PostProcessingEffect::Initialize
(
	const char* i_vertexShaderPath,
	const char* i_fragmentShaderPath,
	const uint8_t i_renderStateBits,
	const eSamplerType texture_samplerType
)
{
	auto result = eae6320::Results::Success;

	// Load Vertex Shader
	{
		if (i_vertexShaderPath && !(result = cShader::Load(i_vertexShaderPath, m_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
		{
			EAE6320_ASSERTF(false, "Can't initialize effect without vertex shader");
			return result;
		}
	}

	// Load Fragment Shader
	{
		if (i_fragmentShaderPath && !(result = cShader::Load(i_fragmentShaderPath, m_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
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

	// Load Sampler
	if (!(result = cSampler::CreateSampler(m_sampler_texture, texture_samplerType)))
	{
		EAE6320_ASSERTF(false, "Can't initialize effect without sampler");
		return result;
	}

	return result;
}

void eae6320::Graphics::PostProcessingEffect::Bind() const
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

	// Bind SRV
	//EAE6320_ASSERT((m_ScenceTexture != nullptr));
	if (m_ScenceTexture) {
		direct3dImmediateContext->PSSetShaderResources(0, 1, &(m_ScenceTexture->m_shaderResourceView));
	}

	//EAE6320_ASSERT((m_DepthTexture != nullptr));
	if (m_DepthTexture) {
		direct3dImmediateContext->PSSetShaderResources(1, 1, &(m_DepthTexture->m_shaderResourceView));
	}

	// Bind Sampler
	m_sampler_texture->Bind(0);
}

eae6320::cResult eae6320::Graphics::PostProcessingEffect::CleanUp()
{
	auto result = eae6320::Results::Success;

	// Release the Shader
	if (m_vertexShader)
	{
		m_vertexShader->DecrementReferenceCount();
		m_vertexShader = nullptr;
	}
	if (m_fragmentShader)
	{
		m_fragmentShader->DecrementReferenceCount();
		m_fragmentShader = nullptr;
	}
	// TODO :
	// Clean up the Sampler
	// Clean up Render State

	return result;
}
