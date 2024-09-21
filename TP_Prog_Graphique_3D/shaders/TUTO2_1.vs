#version 430

// INPUT
layout(location = 1) in vec3 position_in;
layout(location = 2) in vec3 color_in;

// UNIFORM
layout(location = 0) uniform mat4 uModelMatrix;
layout(location = 1) uniform mat4 uViewMatrix;
layout(location = 2) uniform mat4 uProjectionMatrix;
layout(location = 3) out vec3 color;

// MAIN PROGRAM
void main()
{
	color = color_in;
	// - write position with matrix transformations into screen space
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(position_in, 1.0);
}