--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/light_VS.shader", arguments = { "vertex" } },

		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },

        	{ path = "Shaders/Vertex/skybox_VS.shader", arguments = { "vertex" } },

        	{ path = "Shaders/Vertex/shadow_VS.shader", arguments = { "vertex" } },

        	{ path = "Shaders/Fragment/skybox_PS.shader", arguments = { "fragment" } },
	
        	{ path = "Shaders/Vertex/fullScreenTriangle.shader", arguments = { "vertex" } },

        	{ path = "Shaders/Fragment/postProcess_PS.shader", arguments = { "fragment" } },

		{ path = "Shaders/Fragment/postProcess_Distortion_PS.shader", arguments = { "fragment" } },

		{ path = "Shaders/Fragment/postProcess_Lose_PS.shader", arguments = { "fragment" } },

		{ path = "Shaders/Fragment/postProcess_Win_PS.shader", arguments = { "fragment" } },

		{ path = "Shaders/Fragment/FXAA_post_PS.shader", arguments = { "fragment" } },

		{ path = "Shaders/Fragment/light_NormalMap_CSM_PS.shader", arguments = { "fragment" } },
	},

	meshes = 
	{
	    { path = "Meshes/plane.mesh" },
        { path = "Meshes/cube.mesh" },
		{ path = "Meshes/quiver_tree.mesh" },
		{ path = "Meshes/garden_gnome.mesh" },
		{ path = "Meshes/rat.mesh" },
		{ path = "Meshes/cat.mesh" },
		{ path = "Meshes/horse.mesh" },
	},

	textures = 
	{
		{ path = "Textures/ganges_diff_4k.dds" },
		{ path = "Textures/ganges_dx_nmap_4k.dds" },

		{ path = "Textures/quiver_tree_diffuse.dds" },
		{ path = "Textures/quiver_tree_normal_dx.dds" },

		{ path = "Textures/horse_diff.dds" },
		{ path = "Textures/horse_nor.dds" },

		{ path = "Textures/cat_diff.dds" },
		{ path = "Textures/cat_nor.dds" },

		{ path = "Textures/rat_diff.dds" },
		{ path = "Textures/rat_nor.dds" },

		{ path = "Textures/elephant_diff.dds" },
		{ path = "Textures/elephant_nor.dds" },

		{ path = "Textures/garden_gnome_nor.dds" },
		{ path = "Textures/garden_gnome_diff.dds" },

		{ path = "Textures/ganges_diff_4k.dds" },
		{ path = "Textures/ganges_dx_nmap_4k.dds" },

		{ path = "Textures/grasscube1024.dds" },
		{ path = "Textures/desertcube1024.dds" },
	},

	audio = 
	{
		{ path = "Audio/s.wav"},
		{ path = "Audio/testAudio.mp3"},

		{ path = "Audio/bgm.wav"},
		{ path = "Audio/lose.wav"},
		{ path = "Audio/win.wav"},
		{ path = "Audio/evilWin.mp3"},
		{ path = "Audio/Promise.mp3"},
	}
}
