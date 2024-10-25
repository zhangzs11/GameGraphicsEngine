// Includes
//=========

#include <new>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Results/Results.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

#include "cSampler.h"




// Interface
//==========

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cSampler::CreateSampler(eae6320::Graphics::cSampler*& o_sampler)
{
    auto result = eae6320::Results::Success;

    cSampler* newSampler = new (std::nothrow) cSampler();
    if (!newSampler)
    {
        result = eae6320::Results::OutOfMemory;
        return result;
    }

    if (!(result = newSampler->Initialize()))
    {
        delete newSampler;
        newSampler = nullptr;
        return result;
    }

    o_sampler = newSampler;
    return result;
}

eae6320::Graphics::cSampler::~cSampler()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}