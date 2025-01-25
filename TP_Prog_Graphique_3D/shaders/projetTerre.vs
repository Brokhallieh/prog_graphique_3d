#version 430

// INPUT
layout(location = 1) in vec3 position_in;
layout(location = 2) in vec3 normales_in;
layout(location = 3) in vec2 texCoord_in;

// OUTPUT
out vec3 v_pos;
out vec2 texCoord;
out vec3 v_norm; // normal in view space

// UNIFORM
layout(location = 0) uniform mat4 modelMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 projectionMatrix;
layout(location = 3) uniform mat3 uMVnormalMatrix;

// MAIN PROGRAM
void main()
{
	v_pos = (modelMatrix * vec4(position_in, 1.f)).xyz;
	texCoord = texCoord_in;
	v_norm = uMVnormalMatrix * normales_in;
	// - write position with matrix transformations into screen space
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position_in, 1.f);
}