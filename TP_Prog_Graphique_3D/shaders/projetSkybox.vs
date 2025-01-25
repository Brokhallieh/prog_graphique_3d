#version 430

// INPUT
layout(location = 1) in vec3 position_in;

// OUTPUT
out vec3 tex_coord;

// UNIFORM
layout(location = 0) uniform mat4 viewMatrix;
layout(location = 1) uniform mat4 projMatrix;


void main()
{
	// we use the position of the vertex for the 3D tex coords
	tex_coord = position_in;
	vec4 P4 = projMatrix * viewMatrix * vec4(position_in, 1.0);
	gl_Position = P4.xyww;
}