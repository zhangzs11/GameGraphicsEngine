--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/light_VS.shader", arguments = { "vertex" } },

		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },

                { path = "Shaders/Fragment/light_PS.shader", arguments = { "fragment" } },

                { path = "Shaders/Vertex/skybox_VS.shader", arguments = { "vertex" } },

                { path = "Shaders/Fragment/skybox_PS.shader", arguments = { "fragment" } },
	},

	meshes = 
	{
		{ path = "Meshes/helix_gl.mesh" },
                { path = "Meshes/helix_complex.mesh" },
	        { path = "Meshes/pipe_gl.mesh" },
		{ path = "Meshes/plane_gl.mesh" },
                { path = "Meshes/torus_gl.mesh" },
                { path = "Meshes/Alien.mesh" },
                { path = "Meshes/cube.mesh" },
	},

	textures = 
	{
		{ path = "Textures/bricks.dds" },
		{ path = "Textures/flare.dds" },
		{ path = "Textures/grass.dds" },
		{ path = "Textures/stone.dds" },
		{ path = "Textures/water1.dds" },
		{ path = "Textures/water2.dds" },
                { path = "Textures/desertcube1024.dds" },
	},
}
