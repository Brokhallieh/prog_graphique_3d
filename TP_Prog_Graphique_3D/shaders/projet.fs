#version 430
precision highp float;

in vec2 texCoord;

// OUTPUT
out vec4 oFragmentColor;

layout(binding = 0) uniform sampler2D uColor;

// MAIN PROGRAM
void main()
{
	vec3 diffuseColor = texture(uColor, texCoord).rgb;

	oFragmentColor = vec4(diffuseColor, 1.f);
}