// Includes
//=========

#include "Includes.h"
#include "../cEffect.h"
#include "../cShader.h"
#include "../sContext.h"
#include "../cRenderState.h"

#include <Engine/Asserts/Asserts.h>

eae6320::cResult eae6320::Graphics::cEffect::Initialize(const char* i_vertexShaderPath, const char* i_fragmentShaderPath)
{
	auto result = eae6320::Results::Success;

	// Load Vertex Shader
	if (!(result = eae6320::Graphics::cShader::Load(i_vertexShaderPath, m_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
	{
		EAE6320_ASSERTF(false, "Can't initialize effect without vertex shader");
		return result;
	}

	// Load Fragment Shader
	if (!(result = eae6320::Graphics::cShader::Load(i_fragmentShaderPath, m_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
	{
		EAE6320_ASSERTF(false, "Can't initialize effect without fragment shader");
		return result;
	}

	// Initialize Render State
	{
		constexpr auto renderStateBits = []
		{
			uint8_t renderStateBits = 0;

			eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

			return renderStateBits;
		}();
		if (!(result = m_renderState.Initialize(renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
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
	{
		EAE6320_ASSERT((m_vertexShader != nullptr) && (m_vertexShader->m_shaderObject.vertex != nullptr));
		constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
		constexpr unsigned int interfaceCount = 0;
		direct3dImmediateContext->VSSetShader(m_vertexShader->m_shaderObject.vertex, noInterfaces, interfaceCount);
	}

	// Bind Fragment Shader
	{
		EAE6320_ASSERT((m_fragmentShader != nullptr) && (m_fragmentShader->m_shaderObject.fragment != nullptr));
		constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
		constexpr unsigned int interfaceCount = 0;
		direct3dImmediateContext->PSSetShader(m_fragmentShader->m_shaderObject.fragment, noInterfaces, interfaceCount);
	}

	// Bind Render State
	{
		m_renderState.Bind();
	}
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
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

	// TODO: Clean up RenderState
}