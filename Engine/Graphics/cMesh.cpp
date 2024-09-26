// Includes
//=========

#include "cMesh.h"
#include <new>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Results/Results.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
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

	lua_State* luaState = nullptr;

	// Creat a Lua statue
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't create a new Lua state");
			return result;
		}
	}

	// Load lua file
	{
		const auto luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			result = Results::Failure;
			std::cerr << "Failed to load the Lua file: " << lua_tostring(luaState, -1) << std::endl;
			lua_close(luaState);
			return result;
		}
	}

	// Call lua file
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = 1;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;

		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult != LUA_OK)
		{
			result = Results::Failure;
			std::cerr << "Failed to execute the Lua file: " << lua_tostring(luaState, -1) << std::endl;
			lua_close(luaState);
			return result;
		}
	}

	// Make sure return a table
	if (!lua_istable(luaState, -1))
	{
		result = Results::InvalidFile;
		std::cerr << "The Lua file must return a table" << std::endl;
		lua_close(luaState);
		return result;
	}

	// Get vertices data
	lua_getfield(luaState, -1, "vertices");
	if (!lua_istable(luaState, -1))
	{
		result = Results::InvalidFile;
		std::cerr << "The Lua file must contain a 'vertices' table" << std::endl;
		lua_close(luaState);
		return result;
	}

	const auto vertexCount = static_cast<uint16_t>(luaL_len(luaState, -1));
	std::vector<eae6320::Graphics::VertexFormats::sVertex_mesh> vertexData;
	vertexData.reserve(vertexCount * 7); // 3 for position, 4 for color

	for (int i = 1; i <= vertexCount; ++i)
	{
		eae6320::Graphics::VertexFormats::sVertex_mesh v;

		lua_pushinteger(luaState, i);
		lua_gettable(luaState, -2);
		if (lua_istable(luaState, -1))
		{
			// Now get the "position" table
			lua_getfield(luaState, -1, "position");
			if (lua_istable(luaState, -1))
			{
				lua_getfield(luaState, -1, "x");
				// vertexData.push_back(static_cast<float>(lua_tonumber(luaState, -1)));
				v.x = static_cast<float>(lua_tonumber(luaState, -1));
				lua_pop(luaState, 1);

				lua_getfield(luaState, -1, "y");
				// vertexData.push_back(static_cast<float>(lua_tonumber(luaState, -1)));
				v.y = static_cast<float>(lua_tonumber(luaState, -1));
				lua_pop(luaState, 1);

				lua_getfield(luaState, -1, "z");
				// vertexData.push_back(static_cast<float>(lua_tonumber(luaState, -1)));
				v.z = static_cast<float>(lua_tonumber(luaState, -1));
				lua_pop(luaState, 1);
			}
			else
			{
				result = Results::InvalidFile;
				std::cerr << "The Lua file must contain a 'position' table within each vertex" << std::endl;
				lua_close(luaState);
				return result;
			}
			// Pop the "position" table
			lua_pop(luaState, 1);

			// get the "color" table (optional)
			lua_getfield(luaState, -1, "color");
			if (lua_istable(luaState, -1))
			{
				lua_getfield(luaState, -1, "r");
				// vertexData.push_back(static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f));
				v.r = static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f);
				lua_pop(luaState, 1);

				lua_getfield(luaState, -1, "g");
				// vertexData.push_back(static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f));
				v.g = static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f);
				lua_pop(luaState, 1);

				lua_getfield(luaState, -1, "b");
				// vertexData.push_back(static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f));
				v.b = static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f);
				lua_pop(luaState, 1);

				lua_getfield(luaState, -1, "a");
				// vertexData.push_back(static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f));
				v.a = static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f);
				lua_pop(luaState, 1);
			}
			else
			{
				// Use default white color if color is not provided
				// vertexData.push_back(255.0f); // r
				v.r = static_cast<uint8_t>(255.0f);
				v.g = static_cast<uint8_t>(255.0f);
				v.b = static_cast<uint8_t>(255.0f);
				v.a = static_cast<uint8_t>(255.0f);
				// vertexData.push_back(255.0f); // g
				// vertexData.push_back(255.0f); // b
				// vertexData.push_back(255.0f); // a
			}
			// pop color table or empty pop
			lua_pop(luaState, 1);
		}
		vertexData.push_back(v);
		lua_pop(luaState, 1); // pop vertex table
	}
	lua_pop(luaState, 1); // pop vertices table

	// Get indices data 
	lua_getfield(luaState, -1, "indices");
	if (!lua_istable(luaState, -1))
	{
		result = Results::InvalidFile;
		std::cerr << "The Lua file must contain an 'indices' table" << std::endl;
		lua_close(luaState);
		return result;
	}

	const auto indexCount = static_cast<uint16_t>(luaL_len(luaState, -1));
	// Check number of index(not over 65535(uint16_t))
	if (indexCount > std::numeric_limits<uint16_t>::max())
	{
		result = Results::Failure;
		std::cerr << "The Lua file contains too many indices (" << indexCount << "). The maximum allowed is " << std::numeric_limits<uint16_t>::max() << "." << std::endl;
		lua_close(luaState);
		return result;
	}

	std::vector<uint16_t> indexData;
	indexData.reserve(indexCount);

	for (int i = 1; i <= indexCount; ++i)
	{
		lua_pushinteger(luaState, i);
		lua_gettable(luaState, -2);
		indexData.push_back(static_cast<uint16_t>(lua_tointeger(luaState, -1)));
		lua_pop(luaState, 1);
	}
	lua_pop(luaState, 1); // pop indices table

	lua_close(luaState); // close Lua status

	// Create a Mesh and Initialize
	cMesh* newMesh = new (std::nothrow) cMesh();
	if (!newMesh)
	{
		result = Results::OutOfMemory;
		EAE6320_ASSERTF(false, "Couldn't allocate memory for mesh");
		return result;
	}

	if (!(result = newMesh->Initialize(vertexData.data(), vertexCount, indexData.data(), indexCount)))
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