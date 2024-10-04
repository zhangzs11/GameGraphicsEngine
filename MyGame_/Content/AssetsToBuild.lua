--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/standard.shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/standard.shader", arguments = { "fragment" } },

		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/animatedColor.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/loopGradient.shader", arguments = { "fragment" } },
	},

	meshes = 
	{
		{ path = "Meshes/helix_gl.mesh" },
	        { path = "Meshes/pipe_gl.mesh" },
		{ path = "Meshes/plane_gl.mesh" },
                { path = "Meshes/torus_gl.mesh" },
	},
}
