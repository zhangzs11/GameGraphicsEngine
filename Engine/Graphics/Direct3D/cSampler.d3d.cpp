// Includes
//=========

#include "../cSampler.h"

#include "Includes.h"
#include "../sContext.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

eae6320::cResult eae6320::Graphics::cSampler::Initialize(eae6320::Graphics::eSamplerType i_type)
{
    auto result = eae6320::Results::Success;

    auto* const direct3dDevice = sContext::g_context.direct3dDevice;
    EAE6320_ASSERT(direct3dDevice);

    D3D11_SAMPLER_DESC samplerDesc{};

    switch (i_type)
    {
    case eSamplerType::Point:
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        break;
    case eSamplerType::Linear:
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        break;
    case eSamplerType::Anisotropic:
        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.MaxAnisotropy = 16;
        break;

        // maybe more...
    }


    // samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // LINEAR
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    const auto d3dResult = direct3dDevice->CreateSamplerState(&samplerDesc, &m_samplerState);
    if (FAILED(d3dResult))
    {
        result = eae6320::Results::Failure;
        EAE6320_ASSERTF(false, "Fail creating the Sampler (HRESULT %#010x)", d3dResult);
        return result;
    }

    return result;
}

void eae6320::Graphics::cSampler::Bind(unsigned int i_slot) const
{
    auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
    EAE6320_ASSERT(direct3dImmediateContext);
    EAE6320_ASSERT(m_samplerState);

    direct3dImmediateContext->PSSetSamplers(i_slot, 1, &m_samplerState);
}

eae6320::cResult eae6320::Graphics::cSampler::CleanUp()
{
    if (m_samplerState)
    {
        m_samplerState->Release();
        m_samplerState = nullptr;
    }

    return eae6320::Results::Success;
}
