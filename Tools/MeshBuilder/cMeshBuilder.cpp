// Includes
//=========

#include "cMeshBuilder.h"

#include <Engine/Platform/Platform.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <External/Lua/Includes.h>
#include <iostream>
#include <vector>
#include <fstream>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>& i_arguments)
{
	auto result = eae6320::Results::Success;
	lua_State* luaState = nullptr;

    // Create a new Lua state
    {
        luaState = luaL_newstate();
        if (!luaState)
        {
            result = eae6320::Results::OutOfMemory;
            OutputErrorMessageWithFileInfo(m_path_source, "Couldn't create a new Lua state");
            return result;
        }
    }

    // Load and execute the Lua file
    {
        const auto luaResult = luaL_loadfile(luaState, m_path_source);
        if (luaResult != LUA_OK)
        {
            OutputErrorMessageWithFileInfo(m_path_source, lua_tostring(luaState, -1));
            lua_close(luaState);
            return eae6320::Results::Failure;
        }

        constexpr int argumentCount = 0;
        constexpr int returnValueCount = 1;	// Return _everything_ that the file returns
        constexpr int noMessageHandler = 0;

        const auto pcallResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
        if (pcallResult != LUA_OK)
        {
            OutputErrorMessageWithFileInfo(m_path_source, lua_tostring(luaState, -1));
            lua_close(luaState);
            return eae6320::Results::Failure;
        }
    }

    // Ensure the Lua file returns a table
    if (!lua_istable(luaState, -1))
    {
        OutputErrorMessageWithFileInfo(m_path_source, "The Lua file must return a table");
        lua_close(luaState);
        return eae6320::Results::InvalidFile;
    }

    // Get vertices data
    struct sVertex_temp
    {
        float   x, y, z;       // Position
        uint8_t r, g, b, a;    // Color
        float u, v;            // UV
        float nx, ny, nz;      // NORMAL
        float tx, ty, tz;      // TANGENT
    };

    lua_getfield(luaState, -1, "vertices");
    if (!lua_istable(luaState, -1))
    {
        OutputErrorMessageWithFileInfo(m_path_source, "The Lua file must contain a 'vertices' table");
        lua_close(luaState);
        return eae6320::Results::InvalidFile;
    }

    const auto vertexCount = static_cast<uint32_t>(luaL_len(luaState, -1));
    std::vector<sVertex_temp> vertexData;
    vertexData.reserve(vertexCount);

    // Read vertices
    for (uint32_t i = 1; i <= vertexCount; ++i)
    {
        sVertex_temp v;
        lua_pushinteger(luaState, i);
        lua_gettable(luaState, -2); // Get vertex[i]

        if (lua_istable(luaState, -1))
        {
            // Get position
            lua_getfield(luaState, -1, "position");
            if (lua_istable(luaState, -1))
            {
                lua_getfield(luaState, -1, "x");
                v.x = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);

                lua_getfield(luaState, -1, "y");
                v.y = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);

                lua_getfield(luaState, -1, "z");
                v.z = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);
            }
            else
            {
                OutputErrorMessageWithFileInfo(m_path_source, "The Lua file must contain a 'position' table within each vertex");
                lua_close(luaState);
                return eae6320::Results::InvalidFile;
            }
            // Pop the "position" table
            lua_pop(luaState, 1);

            // Get color
            lua_getfield(luaState, -1, "color");
            if (lua_istable(luaState, -1))
            {
                lua_getfield(luaState, -1, "r");
                v.r = static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f);
                lua_pop(luaState, 1);

                lua_getfield(luaState, -1, "g");
                v.g = static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f);
                lua_pop(luaState, 1);

                lua_getfield(luaState, -1, "b");
                v.b = static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f);
                lua_pop(luaState, 1);

                lua_getfield(luaState, -1, "a");
                v.a = static_cast<uint8_t>(lua_tonumber(luaState, -1) * 255.0f + 0.5f);
                lua_pop(luaState, 1);
            }
            else
            {
                // Use default white color if color is not provided
                v.r = static_cast<uint8_t>(255.0f);
                v.g = static_cast<uint8_t>(255.0f);
                v.b = static_cast<uint8_t>(255.0f);
                v.a = static_cast<uint8_t>(255.0f);
            }
            lua_pop(luaState, 1); // Pop color

            // Get UV
            lua_getfield(luaState, -1, "texcoord");
            if (lua_istable(luaState, -1))
            {
                lua_getfield(luaState, -1, "u");
                v.u = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);

                lua_getfield(luaState, -1, "v");
                v.v = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);
            }
            else
            {
                // Use default uv if uv is not provided
                v.u = static_cast<float>(0.0f);
                v.v = static_cast<float>(0.0f);
            }
            lua_pop(luaState, 1); // Pop UV

            // Get normal
            lua_getfield(luaState, -1, "normal");
            if (lua_istable(luaState, -1))
            {
                lua_getfield(luaState, -1, "nx");
                v.nx = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);

                lua_getfield(luaState, -1, "ny");
                v.ny = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);

                lua_getfield(luaState, -1, "nz");
                v.nz = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);
            }
            else
            {
                OutputErrorMessageWithFileInfo(m_path_source, "The Lua file must contain a 'normal' table within each vertex");
                lua_close(luaState);
                return eae6320::Results::InvalidFile;
            }
            lua_pop(luaState, 1); // Pop normal

            // Get tangent
            lua_getfield(luaState, -1, "tangent");
            if (lua_istable(luaState, -1))
            {
                lua_getfield(luaState, -1, "tx");
                v.tx = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);

                lua_getfield(luaState, -1, "ty");
                v.ty = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);

                lua_getfield(luaState, -1, "tz");
                v.tz = static_cast<float>(lua_tonumber(luaState, -1));
                lua_pop(luaState, 1);
            }
            else
            {
                OutputErrorMessageWithFileInfo(m_path_source, "The Lua file must contain a 'tangent' table within each vertex");
                lua_close(luaState);
                return eae6320::Results::InvalidFile;
            }
            lua_pop(luaState, 1); // Pop tangent

            // Add vertex to vextex vector
            vertexData.push_back(v);
        }

        lua_pop(luaState, 1); // Pop vertex[i]
    }
    lua_pop(luaState, 1); // Pop vertices table

    // Get indices data
    lua_getfield(luaState, -1, "indices");
    if (!lua_istable(luaState, -1))
    {
        OutputErrorMessageWithFileInfo(m_path_source, "The Lua file must contain an 'indices' table");
        lua_close(luaState);
        return eae6320::Results::InvalidFile;
    }

    const auto indexCount = static_cast<uint32_t>(luaL_len(luaState, -1));
    
    bool use32BitIndex = (vertexCount > std::numeric_limits<uint16_t>::max());

    // Choose between 16-bit and 32-bit index based on vertex count
    if (use32BitIndex)
    {
        std::vector<uint32_t> indexData32Bits;
        indexData32Bits.reserve(indexCount);

        // Read indices as uint32_t
        for (uint32_t i = 1; i <= indexCount; ++i)
        {
            lua_pushinteger(luaState, i);
            lua_gettable(luaState, -2);
            indexData32Bits.push_back(static_cast<uint32_t>(lua_tointeger(luaState, -1)));
            lua_pop(luaState, 1); // Pop index[i]
        }
        lua_pop(luaState, 1); // Pop indices table

        // Write binary data to target file
        std::ofstream binaryFile(m_path_target, std::ios::binary);
        if (!binaryFile)
        {
            OutputErrorMessageWithFileInfo(m_path_target, "Failed to open binary file for writing");
            return eae6320::Results::Failure;
        }

        // Write vertex count
        binaryFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));

        // Write vertex data
        binaryFile.write(reinterpret_cast<const char*>(vertexData.data()), vertexCount * sizeof(sVertex_temp));

        // Write index count
        binaryFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));

        // Write 32-bit index data
        binaryFile.write(reinterpret_cast<const char*>(indexData32Bits.data()), indexCount * sizeof(uint32_t));

        binaryFile.close();
    }
    else
    {
        std::vector<uint16_t> indexData16Bits;
        indexData16Bits.reserve(indexCount);

        // Read indices
        for (uint32_t i = 1; i <= indexCount; ++i)
        {
            lua_pushinteger(luaState, i);
            lua_gettable(luaState, -2);
            indexData16Bits.push_back(static_cast<uint16_t>(lua_tointeger(luaState, -1)));
            lua_pop(luaState, 1); // Pop index[i]
        }
        // Pop indices table
        lua_pop(luaState, 1);

        // Close Lua state
        lua_close(luaState);

        // Write binary data to target file
        std::ofstream binaryFile(m_path_target, std::ios::binary);
        if (!binaryFile)
        {
            OutputErrorMessageWithFileInfo(m_path_target, "Failed to open binary file for writing");
            return eae6320::Results::Failure;
        }

        // Write vertex count
        binaryFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));

        // Write vertex data
        binaryFile.write(reinterpret_cast<const char*>(vertexData.data()), vertexCount * sizeof(sVertex_temp));

        // Write index count
        binaryFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));

        // Write index data
        binaryFile.write(reinterpret_cast<const char*>(indexData16Bits.data()), indexCount * sizeof(uint16_t));

        binaryFile.close();
    }

	return eae6320::Results::Success;
}
