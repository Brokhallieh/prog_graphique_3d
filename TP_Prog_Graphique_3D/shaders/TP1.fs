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
	oFragmentColor = vec4(position_out, color[2], 1.f); // [values are between 0.0 and 1.0]
}