// Includes
//=========

#include "cEffect.h"
#include <new>


// Interface
//==========

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cEffect::CreateEffect(eae6320::Graphics::cEffect*& o_effect, const char* i_vertexShaderPath, 
											              const char* i_fragmentShaderPath, const uint8_t i_renderStateBits)
{
	auto result = eae6320::Results::Success;

	// allocate
	cEffect* newEffect = new (std::nothrow) cEffect();
	if (!newEffect)
	{
		result = eae6320::Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for effect");
		return result;
	}

	// initialize a new instance of class cEffect
	if (!(result = newEffect->Initialize(i_vertexShaderPath, i_fragmentShaderPath, i_renderStateBits)))
	{
		delete newEffect;
		newEffect = nullptr;
		return result;
	}

	o_effect = newEffect;

	return result;
}








// Implementation
//===============

// Initialize / Clean Up
//----------------------

eae6320::Graphics::cEffect::~cEffect()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}