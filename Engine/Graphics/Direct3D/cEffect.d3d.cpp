//// Includes
////=========
//
//#include "Includes.h"
//#include "../cEffect.h"
//#include "../cShader.h"
//#include "../sContext.h"
//#include "../cRenderState.h"
//#include "../cView.h"
//
//#include <Engine/Asserts/Asserts.h>
//
//eae6320::cResult eae6320::Graphics::cEffect::Initialize(const char* i_vertexShaderPath, const char* i_fragmentShaderPath, 
//	                                                    const uint8_t i_renderStateBits, 
//	                                                    const std::vector<std::string>& texturePaths,
//	                                                    const std::vector<eSamplerType>& samplerTypes)
//{
//	auto result = eae6320::Results::Success;
//
//	// Load Vertex Shader
//	{
//		if (i_vertexShaderPath && !(result = cShader::Load(i_vertexShaderPath, m_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
//		{
//			EAE6320_ASSERTF(false, "Can't initialize effect without vertex shader");
//			return result;
//		}
//	}
//
//	// Load Fragment Shader
//	{
//		if (i_fragmentShaderPath && !(result = cShader::Load(i_fragmentShaderPath, m_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
//		{
//			EAE6320_ASSERTF(false, "Can't initialize effect without pixel shader");
//			return result;
//		}
//	}
//
//	// Initialize Render State
//	if (!(result = m_renderState.Initialize(i_renderStateBits)))
//	{
//		EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
//		return result;
//	}
//
//	// Load Texture
//	for (const auto texturePath : texturePaths)
//	{
//		cTexture* texture = nullptr;
//		if (!(result = cTexture::CreateTextureDDS(texture, texturePath.c_str())))
//		{
//			EAE6320_ASSERTF(false, "Can't initialize effect without texture");
//			return result;
//		}
//		// cTexture::CreateTextureWIC(m_texture, i_texturePath);
//		m_textures.push_back(texture);
//	}
//
//	// Load Sampler
//	for (const auto samplerType : samplerTypes)
//	{
//		cSampler* sampler = nullptr;
//		if (!(result = cSampler::CreateSampler(sampler, samplerType)))
//		{
//			EAE6320_ASSERTF(false, "Can't initialize effect without sampler");
//			return result;
//		}
//		m_samplers.push_back(sampler);
//	}
//
//	return result;
//}
//
//void eae6320::Graphics::cEffect::Bind() const
//{
//	auto* const direct3dImmediateContext = eae6320::Graphics::sContext::g_context.direct3dImmediateContext;
//	EAE6320_ASSERT(direct3dImmediateContext != nullptr);
//
//	// Bind Vertex Shader
//	EAE6320_ASSERT((m_vertexShader != nullptr) && (m_vertexShader->m_shaderObject.vertex != nullptr));
//	constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
//	constexpr unsigned int interfaceCount = 0;
//	direct3dImmediateContext->VSSetShader(m_vertexShader->m_shaderObject.vertex, noInterfaces, interfaceCount);
//
//	// Bind Pixel Shader
//	if (m_fragmentShader) {
//		EAE6320_ASSERT((m_fragmentShader != nullptr) && (m_fragmentShader->m_shaderObject.fragment != nullptr));
//		direct3dImmediateContext->PSSetShader(m_fragmentShader->m_shaderObject.fragment, noInterfaces, interfaceCount);
//	}
//	else {
//		direct3dImmediateContext->PSSetShader(NULL, noInterfaces, interfaceCount);
//	}
//	// EAE6320_ASSERT((m_fragmentShader != nullptr) && (m_fragmentShader->m_shaderObject.fragment != nullptr));
//	// direct3dImmediateContext->PSSetShader(m_fragmentShader->m_shaderObject.fragment, noInterfaces, interfaceCount);
//
//	// Bind Render State
//	m_renderState.Bind();
//
//	// Bind each Texture
//	for (size_t i = 0; i < m_textures.size(); ++i) {
//		if (m_textures[i]) {
//			m_textures[i]->Bind(static_cast<unsigned int>(i));
//		}
//	}
//
//	// Bind each Sampler
//	for (size_t i = 0; i < m_samplers.size(); ++i) {
//		if (m_samplers[i]) {
//			m_samplers[i]->Bind(static_cast<unsigned int>(i));
//		}
//	}
//
//	// Bind Shadow Map if available (for light effect)
//	if (m_shadowMapView && m_shadowMapView->GetshaderResourceView()) {
//		ID3D11ShaderResourceView* shaderResourceViews[] = { m_shadowMapView->GetshaderResourceView() };
//		direct3dImmediateContext->PSSetShaderResources(static_cast<unsigned int>(m_textures.size()), 1, shaderResourceViews);
//	}
//
//	// 
//
//}
//
//eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
//{
//	auto result = eae6320::Results::Success;
//
//	// Release the Vertex Shader
//	if (m_vertexShader)
//	{
//		m_vertexShader->DecrementReferenceCount();
//		m_vertexShader = nullptr;
//	}
//
//	// Release the Pixel Shader
//	if (m_fragmentShader)
//	{
//		m_fragmentShader->DecrementReferenceCount();
//		m_fragmentShader = nullptr;
//	}
//
//	// Release the Texture
//	for (auto* texture : m_textures) {
//		if (texture) {
//			texture->DecrementReferenceCount();
//			texture = nullptr;
//		}
//	}
//	// Clean up Render State
//	// TODO :
//	// Clean up the Sampler
//	return result;
//}