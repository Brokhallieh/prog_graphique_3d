#version 430

// INPUT
layout(location = 1) in vec3 position_in;
layout(location = 3) in vec2 texCoord_in;

// OUTPUT
out vec2 texCoord;

// UNIFORM
layout(location = 0) uniform mat4 modelMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 projectionMatrix;

// MAIN PROGRAM
void main()
{
	texCoord = texCoord_in;
	// - write position with matrix transformations into screen space
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position_in, 1.f);
}