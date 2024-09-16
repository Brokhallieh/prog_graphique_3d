#version 430
precision highp float;
layout(location = 0) uniform vec3 color;
layout(location = 3) in vec2 position_out;

// OUTPUT
// - the default GL framebuffer is a RGBA color buffer
// - you can choose whatever name you want
// - variable is prefixed by "out"
out vec4 oFragmentColor;

// MAIN PROGRAM
void main()
{
	// MANDATORY
	// - a fragment shader MUST write an RGBA color
	oFragmentColor = vec4(mix(position_out[0], 1.f, color[0]), mix(position_out[1], 1.f, color[1]), mix(mix(position_out[0], position_out[1], 0.5f), color[2], color[2]), 1.f); // [values are between 0.0 and 1.0]
}