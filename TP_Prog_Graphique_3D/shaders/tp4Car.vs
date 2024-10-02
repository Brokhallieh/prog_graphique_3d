#version 430

// INPUT
layout(location = 1) in vec3 position_in;
layout(location = 2) in vec3 normales_in;

// OUTPUT
out vec3 v_pos; // position in view space
out vec3 v_norm; // normal in view space

// UNIFORM
layout(location = 0) uniform mat4 uModelMatrix;
layout(location = 1) uniform mat4 uViewMatrix;
layout(location = 2) uniform mat4 uProjectionMatrix;
layout(location = 3) uniform mat3 uMVnormalMatrix;

// MAIN PROGRAM
void main()
{
	// get the normal in view space
	v_norm = uMVnormalMatrix * normales_in;
	// get the position in view space
	vec4 viewPos = uViewMatrix * uModelMatrix * vec4(position_in, 1.0);
	v_pos = viewPos.xyz;
	// - write position with matrix transformations into screen space
	gl_Position = uProjectionMatrix * viewPos;
}