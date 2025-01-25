#version 430

precision highp float;

// INPUT
in vec3 tex_coord;

// UNIFORM BINDING
layout(binding = 0) uniform samplerCube uSkyTex;

// OUTPUT
out vec4 oFragmentColor;

void main()
{	
	// The texture cube map is sampled with 3D coords
	oFragmentColor = texture(uSkyTex, tex_coord);
}