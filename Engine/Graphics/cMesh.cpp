// Includes
//=========

#include "cMesh.h"
#include <new>


// Interface
//==========

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cMesh::CreateMesh(eae6320::Graphics::cMesh*& o_mesh, const void* i_vertexData, const uint16_t i_vertexCount,
	                                                  const uint16_t* i_indexData, const uint16_t i_indexCount)
{
	auto result = eae6320::Results::Success;

	// allocate
	cMesh* newMesh = new (std::nothrow) cMesh();
	if (!newMesh)
	{
		result = eae6320::Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for mesh");
		return result;
	}

	// initialize a new instance of class cEffect
	if (!(result = newMesh->Initialize(i_vertexData, i_vertexCount, i_indexData, i_indexCount)))
	{
		delete newMesh;
		newMesh = nullptr;
		return result;
	}

	o_mesh = newMesh;

	return result;
}





// Implementation
//===============

// Initialize / Clean Up
//----------------------

eae6320::Graphics::cMesh::~cMesh()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}