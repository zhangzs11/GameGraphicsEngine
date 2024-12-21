// Includes
//=========
#include "../Direct3D/Includes.h"
#include "DeferredRenderingEffect.d3d.h"
#include "../cShader.h"
#include "../sContext.h"
#include "../cRenderState.h"
#include "../cView.h"
#include <new>

// -----------------------------------------------
// GEOMETRY
// -----------------------------------------------

eae6320::cResult eae6320::Graphics::DeferredRenderingEffect_Geometry::CreateDeferredRenderingEffect_Geometry
(
	DeferredRenderingEffect_Geometry*& o_effect,
	const char* i_vertexShaderPath,
	const char* i_fragmentShaderPath,
	const uint8_t i_renderStateBits,
	const std::string& texture0Path,
	const eSamplerType texture_samplerType
)
{
	auto result = eae6320::Results::Success;

	// allocate
	DeferredRenderingEffect_Geometry* newEffect = new (std::nothrow) DeferredRenderingEffect_Geometry();
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
		texture0Path,
		texture_samplerType)))
	{
		delete newEffect;
		newEffect = nullptr;
		return result;
	}

	o_effect = newEffect;

	return result;
}

eae6320::Graphics::DeferredRenderingEffect_Geometry::~DeferredRenderingEffect_Geometry()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}

eae6320::cResult eae6320::Graphics::DeferredRenderingEffect_Geometry::Initialize
(
	const char* i_vertexShaderPath,
	const char* i_fragmentShaderPath,
	const uint8_t i_renderStateBits,
	const std::string& texture0Path,
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

	// Load Texture
	{
		if (!texture0Path.empty())
		{
			if (!(result = cTexture::CreateTextureDDS(m_texture0, texture0Path.c_str())))
			{
				EAE6320_ASSERTF(false, "Can't initialize effect without texture");
				return result;
			}
		}
		else
		{
			m_texture0 = nullptr; // or handle accordingly if you want to set a default texture or skip this texture
		}
	}

	// Load Sampler
	if (!(result = cSampler::CreateSampler(m_sampler_texture, texture_samplerType)))
	{
		EAE6320_ASSERTF(false, "Can't initialize effect without sampler");
		return result;
	}

	return result;
}

void eae6320::Graphics::DeferredRenderingEffect_Geometry::Bind() const
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
	if (m_texture0) {
		m_texture0->Bind(0);
	}

	// Bind Sampler
	m_sampler_texture->Bind(0);
}

eae6320::cResult eae6320::Graphics::DeferredRenderingEffect_Geometry::CleanUp()
{
	auto result = eae6320::Results::Success;

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

	return result;
}

// -----------------------------------------------
// LIGHTING
// -----------------------------------------------

eae6320::cResult eae6320::Graphics::DeferredRenderingEffect_Lighting::CreateDeferredRenderingEffect_Lighting
(
	DeferredRenderingEffect_Lighting*& o_effect,
	const char* i_vertexShaderPath,
	const char* i_fragmentShaderPath,
	const uint8_t i_renderStateBits
)
{
	auto result = eae6320::Results::Success;

	// allocate
	DeferredRenderingEffect_Lighting* newEffect = new (std::nothrow) DeferredRenderingEffect_Lighting();
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
		i_renderStateBits)))
	{
		delete newEffect;
		newEffect = nullptr;
		return result;
	}

	o_effect = newEffect;

	return result;
}

eae6320::Graphics::DeferredRenderingEffect_Lighting::~DeferredRenderingEffect_Lighting()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}

eae6320::cResult eae6320::Graphics::DeferredRenderingEffect_Lighting::Initialize
(
	const char* i_vertexShaderPath,
	const char* i_fragmentShaderPath,
	const uint8_t i_renderStateBits
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

	return result;
}

void eae6320::Graphics::DeferredRenderingEffect_Lighting::Bind() const
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

	// Bind GBuffer
	EAE6320_ASSERT((m_GBuffer_01 != nullptr));
	direct3dImmediateContext->PSSetShaderResources(1, 1, &(m_GBuffer_01->m_shaderResourceView));

	EAE6320_ASSERT((m_GBuffer_02 != nullptr));
	direct3dImmediateContext->PSSetShaderResources(2, 1, &(m_GBuffer_02->m_shaderResourceView));

	EAE6320_ASSERT((m_GBuffer_03 != nullptr));
	direct3dImmediateContext->PSSetShaderResources(3, 1, &(m_GBuffer_03->m_shaderResourceView));

	EAE6320_ASSERT((m_depthTexture != nullptr));
	direct3dImmediateContext->PSSetShaderResources(4, 1, &(m_depthTexture->m_shaderResourceView));

	// Bind Structed Buffer
	EAE6320_ASSERT((m_structedBuffer_light != nullptr));
	direct3dImmediateContext->PSSetShaderResources(5, 1, &(m_structedBuffer_light->m_shaderResourceView));
}

eae6320::cResult eae6320::Graphics::DeferredRenderingEffect_Lighting::CleanUp()
{
	auto result = eae6320::Results::Success;

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