#version 430

layout(location = 1) in vec3 position_in;

layout(location = 1) uniform mat4 model;
layout(location = 2) uniform mat4 view;
layout(location = 3) uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * vec4(position_in, 1.0);
}