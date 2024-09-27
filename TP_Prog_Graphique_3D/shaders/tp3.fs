#version 430
precision highp float;

//INPUT
in vec4 texture_color;

// OUTPUT
out vec4 oFragmentColor;

// UNIFORM
layout(location = 3) uniform vec4 colorMap[255];

// MAIN PROGRAM
void main()
{
	vec4 finalColor = colorMap[int(texture_color.r*255)];
	oFragmentColor = finalColor;
}