#version 430

// INPUT
layout(location = 1) in vec3 position_in;
layout(location = 2) in vec3 normal_in;
layout(location = 3) in vec3 centers_in;

// UNIFORM
// - Camera matrices
layout(location = 1) uniform mat4 uViewMatrix;
layout(location = 2) uniform mat4 uProjectionMatrix;
layout(location = 3) uniform mat3 uNormalMatrix;

// OUTPUT
out vec3 v_position;
out vec3 v_normal;

// MAIN PROGRAM
void main()
{
	vec4 view_pos = uViewMatrix * vec4(centers_in + 0.03 * position_in, 1.0); // the position is moved according to a center point and we apply a rescale
	v_position = view_pos.xyz; // in View space
	v_normal = uNormalMatrix * (centers_in + 0.03 * normal_in); // in View space

	gl_Position = uProjectionMatrix * view_pos;
}