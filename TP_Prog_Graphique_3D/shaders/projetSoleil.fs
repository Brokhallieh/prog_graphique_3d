#version 430
precision highp float;

#define PI 3.141592653589793f

// INPUT
in vec2 texCoord;

// OUTPUT
out vec4 oFragmentColor;

// UNIFORM BINDING
layout(binding = 0) uniform sampler2D uColor;

// MAIN PROGRAM
void main()
{
	oFragmentColor = texture(uColor, texCoord);
}