#version 430

layout(location = 1) in vec3 position_in;
layout(location = 2) in vec3 normal_in;

layout (location = 0) uniform mat4 modelMatrix;
layout (location = 1) uniform mat4 viewMatrix;
layout (location = 2) uniform mat4 projectionMatrix;
layout (location = 3) uniform mat3 normalMatrix;

out vec3 w_pos;
out vec3 w_normal;

void main()
{
	vec4 P4 = modelMatrix * vec4(position_in, 1);
	gl_Position = projectionMatrix * viewMatrix * P4;

	w_pos = P4.xyz;
	w_normal = normalMatrix * normal_in;
}