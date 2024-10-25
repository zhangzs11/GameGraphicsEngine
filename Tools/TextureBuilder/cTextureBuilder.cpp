// Includes
//=========

#include "cTextureBuilder.h"

#include <Engine/Platform/Platform.h>
#include <Tools/AssetBuildLibrary/Functions.h>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cTextureBuilder::Build(const std::vector<std::string>& i_arguments)
{
	auto result = eae6320::Platform::CopyFile(m_path_source, m_path_target);
	if (!result)
	{
		OutputErrorMessageWithFileInfo(m_path_source, "Failed to copy textures file");
		return result;
	}

	return eae6320::Results::Success;
}
