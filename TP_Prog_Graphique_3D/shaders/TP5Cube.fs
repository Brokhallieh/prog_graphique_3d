#version 430
precision highp float;

// INPUT
in vec2 v_textureCoord;

// OUPUT
out vec4 oFragmentColor;

// UNIFORM
layout(binding = 0) uniform sampler2D uSampler;

// MAIN PROGRAM
void main()
{
	vec4 textureColor = texture(uSampler, v_textureCoord);

	oFragmentColor = textureColor;
}