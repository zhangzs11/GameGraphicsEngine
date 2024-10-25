// Includes
//=========

#include "cMesh.h"
#include <new>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Results/Results.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Graphics/VertexFormats.h>
#include <External/Lua/Includes.h>
#include <iostream>
#include <vector>

// Interface
//==========

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cMesh::CreateMesh(eae6320::Graphics::cMesh*& o_mesh, const char* const i_path)
{
	auto result = eae6320::Results::Success;

	// Load the binary mesh file
	Platform::sDataFromFile dataFromFile;
	if (!(result = Platform::LoadBinaryFile(i_path, dataFromFile)))
	{
		Logging::OutputError("Failed to load binary mesh file: %s", i_path);
		return result;
	}

	uintptr_t currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
	const auto finalOffset = currentOffset + dataFromFile.size;

    // Read the vertex count
    uint32_t vertexCount;
    memcpy(&vertexCount, reinterpret_cast<void*>(currentOffset), sizeof(vertexCount));
    currentOffset += sizeof(vertexCount); // Move to the next block

    // Calculate the size of the vertex data
    const size_t vertexDataSize = vertexCount * sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh);
    if ((currentOffset + vertexDataSize) > finalOffset) // Check if the file is large enough for vertex data
    {
        Logging::OutputError("The binary mesh file is too small for the vertex data: %s", i_path);
        return eae6320::Results::InvalidFile;
    }

    // Get the vertex data
    const auto* const vertexArray = reinterpret_cast<const eae6320::Graphics::VertexFormats::sVertex_mesh*>(currentOffset);
    currentOffset += vertexDataSize; // Move to the next block (index count)

    // Read the index count
    uint32_t indexCount;
    memcpy(&indexCount, reinterpret_cast<void*>(currentOffset), sizeof(indexCount));
    currentOffset += sizeof(indexCount); // Move to the next block (index data)

    // Determine whether to use 16-bit or 32-bit indices based on vertex count
    bool use32BitIndex = (vertexCount > std::numeric_limits<uint16_t>::max());

    // Read the index data
    void* indexArray = nullptr;
    if (use32BitIndex)
    {
        const size_t indexDataSize = indexCount * sizeof(uint32_t);
        if ((currentOffset + indexDataSize) > finalOffset)
        {
            Logging::OutputError("The binary mesh file is too small for the 32-bit index data: %s", i_path);
            return eae6320::Results::InvalidFile;
        }
        indexArray = reinterpret_cast<uint32_t*>(currentOffset);
    }
    else
    {
        const size_t indexDataSize = indexCount * sizeof(uint16_t);
        if ((currentOffset + indexDataSize) > finalOffset)
        {
            Logging::OutputError("The binary mesh file is too small for the 16-bit index data: %s", i_path);
            return eae6320::Results::InvalidFile;
        }
        indexArray = reinterpret_cast<uint16_t*>(currentOffset);
    }

    // Allocate memory for the mesh
    cMesh* newMesh = new (std::nothrow) cMesh();
    if (!newMesh)
    {
        result = eae6320::Results::OutOfMemory;
        Logging::OutputError("Couldn't allocate memory for the mesh: %s", i_path);
        return result;
    }

    // Initialize the mesh with the vertex and index data
    if (!(result = newMesh->Initialize(vertexArray, vertexCount, indexArray, indexCount, use32BitIndex)))
    {
        Logging::OutputError("Mesh initialization failed for the file: %s", i_path);
        delete newMesh;
        newMesh = nullptr;
        return result;
    }

	o_mesh = newMesh;

	return result;
}

eae6320::Graphics::cMesh::~cMesh()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}