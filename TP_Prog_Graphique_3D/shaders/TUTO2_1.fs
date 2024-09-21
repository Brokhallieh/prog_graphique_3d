#version 430
precision highp float;
layout(location = 3) in vec3 color;

// OUTPUT
out vec4 oFragmentColor;

// MAIN PROGRAM
void main()
{
	oFragmentColor = vec4(color, 0.f);
}