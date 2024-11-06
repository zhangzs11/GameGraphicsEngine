// Includes
//=========
#include "../Direct3D/Includes.h"
#include "ShadowEffect.d3d.h"
#include "../cShader.h"
#include "../sContext.h"
#include "../cRenderState.h"
#include <new>

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::ShadowEffect::CreateShadowEffect(
	ShadowEffect*& o_effect,
	const char* i_vertexShaderPath,
	const uint8_t i_renderStateBits
)
{
	auto result = eae6320::Results::Success;

	// allocate
	ShadowEffect* newEffect = new (std::nothrow) ShadowEffect();
	if (!newEffect)
	{
		result = eae6320::Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for effect");
		return result;
	}

	// initialize a new instance of class cEffect
	if (!(result = newEffect->Initialize(
		i_vertexShaderPath,
		i_renderStateBits)))
	{
		delete newEffect;
		newEffect = nullptr;
		return result;
	}

	o_effect = newEffect;

	return result;
}

eae6320::Graphics::ShadowEffect::~ShadowEffect()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}

eae6320::cResult eae6320::Graphics::ShadowEffect::Initialize
(
	const char* i_vertexShaderPath,
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

	// Initialize Render State
	if (!(result = m_renderState.Initialize(i_renderStateBits)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
		return result;
	}
	
	// Initialize Depth Stencil View
	

	// Initialize Shader Resource View



	return result;
}

void eae6320::Graphics::ShadowEffect::Bind() const
{
	auto* const direct3dImmediateContext = eae6320::Graphics::sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext != nullptr);

	// Bind Vertex Shader
	EAE6320_ASSERT((m_vertexShader != nullptr) && (m_vertexShader->m_shaderObject.vertex != nullptr));
	constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
	constexpr unsigned int interfaceCount = 0;
	direct3dImmediateContext->VSSetShader(m_vertexShader->m_shaderObject.vertex, noInterfaces, interfaceCount);

	// Bind Pixel Shader (NULL)
	direct3dImmediateContext->PSSetShader(NULL, noInterfaces, interfaceCount);

	// Bind Render State
	m_renderState.Bind();

}

eae6320::cResult eae6320::Graphics::ShadowEffect::CleanUp()
{
	auto result = eae6320::Results::Success;

	// Release the Vertex Shader
	if (m_vertexShader)
	{
		m_vertexShader->DecrementReferenceCount();
		m_vertexShader = nullptr;
	}
	// TODO :
	// Clean up Render State
	// Clean up the SRV and RTV

	return result;
}
