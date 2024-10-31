// Includes
//=========

#include <new>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Results/Results.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

#include "cTexture.h"


// Interface
//==========

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cTexture::CreateTextureDDS(eae6320::Graphics::cTexture*& o_texture, const char* const i_path)
{
	auto result = Results::Success;

	cTexture* newTexture = new (std::nothrow) cTexture();
	if (!newTexture)
	{
		result = eae6320::Results::OutOfMemory;
		eae6320::Logging::OutputError("Couldn't allocate memory for textureWIC");
		return result;
	}

	if (!(result = newTexture->InitializeDDS(i_path)))
	{
		eae6320::Logging::OutputError("Texture initialization failed for the WIC file: %s", i_path);
		delete newTexture;
		newTexture = nullptr;
		return result;
	}

	o_texture = newTexture;

	return result;
}

eae6320::Graphics::cTexture::~cTexture()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}