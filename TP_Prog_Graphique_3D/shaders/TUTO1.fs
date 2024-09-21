#version 430
precision highp float;

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
	oFragmentColor = vec4(0.8f, 0.5f, 0.2f, 1.f);
}