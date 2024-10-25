/*
	This class builds textures
*/

#ifndef EAE6320_CTEXTUREBUILDER_H
#define EAE6320_CTEXTUREBUILDER_H

// Includes
//=========

#include <Tools/AssetBuildLibrary/iBuilder.h>

#include <Engine/Graphics/Configuration.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cTextureBuilder final : public iBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			cResult Build(const std::vector<std::string>& i_arguments) final;

			// Implementation
			//===============
		};
	}
}

#endif	// EAE6320_CTEXTUREBUILDER_H
