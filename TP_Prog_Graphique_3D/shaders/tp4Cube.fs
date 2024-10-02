#version 430
precision highp float;

#define M_PI 3.14159265358979

// IN
in vec3 position;

// OUTPUT
out vec4 oFragmentColor;

layout(binding = 0) uniform samplerCube uSampler; 

// MAIN PROGRAM
void main()
{
	oFragmentColor = texture(uSampler, position);
}