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

		{ path = "Shaders/Fragment/FXAA_post_PS.shader", arguments = { "fragment" } },

		{ path = "Shaders/Fragment/light_NormalMap_CSM_PS.shader", arguments = { "fragment" } },
	},

	meshes = 
	{
	    { path = "Meshes/plane.mesh" },
        { path = "Meshes/cube.mesh" },
		{ path = "Meshes/quiver_tree.mesh" },
		{ path = "Meshes/quiver_tree_all_change.mesh" },
		{ path = "Meshes/table.mesh" },
		{ path = "Meshes/marble_bust.mesh" },
		{ path = "Meshes/marble_bust_big.mesh" },
		{ path = "Meshes/wall.mesh" },
	},

	textures = 
	{
		{ path = "Textures/bricks.dds" },
		{ path = "Textures/bricks_nmap.dds" },
		{ path = "Textures/stones.dds" },
		{ path = "Textures/stones_nmap.dds" },
		{ path = "Textures/grass.dds" },
		{ path = "Textures/stone.dds" },
		{ path = "Textures/water1.dds" },
		{ path = "Textures/water2.dds" },
        { path = "Textures/desertcube1024.dds" },
		{ path = "Textures/house.dds" },
		{ path = "Textures/ganges_diff_4k.dds" },
		{ path = "Textures/ganges_dx_nmap_4k.dds" },
		{ path = "Textures/quiver_tree_diffuse.dds" },
		{ path = "Textures/quiver_tree_normal_dx.dds" },

		{ path = "Textures/marble_bust_diff.dds" },
		{ path = "Textures/marble_bust_nmap.dds" },
		{ path = "Textures/wooden_table_diff.dds" },
		{ path = "Textures/wooden_table_nmap.dds" },

		{ path = "Textures/grasscube1024.dds" },
	},
}
