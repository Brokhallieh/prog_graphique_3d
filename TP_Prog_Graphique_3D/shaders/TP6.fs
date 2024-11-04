#version 430
precision highp float;

// INPUT
in vec3 v_position;
in vec3 v_normal;

// UNIFORM
// Material (BRDF: bidirectional reflectance distribution function)
layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 albedo;
layout(location = 4) uniform vec3 uAlbedo; // diffuse

// OUTPUT
out vec4 oFragmentColor;

// MAIN PROGRAM
void main()
{
	position = v_position;
	normal = v_normal;
	albedo = uAlbedo;
}