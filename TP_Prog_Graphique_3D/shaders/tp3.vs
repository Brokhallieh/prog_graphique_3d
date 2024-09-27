#version 430

// INPUT
layout(location = 1) in vec3 position_in;
layout(location = 2) in vec2 texture_coord_in;

// OUTPUT
out vec4 texture_color;

// UNIFORM
layout(location = 0) uniform mat4 uModelMatrix;
layout(location = 1) uniform mat4 uViewMatrix;
layout(location = 2) uniform mat4 uProjectionMatrix;

// BINDING
layout(binding = 0) uniform sampler2D uSampler;

// MAIN PROGRAM
void main()
{
	texture_color = texture(uSampler, texture_coord_in);
	// - write position with matrix transformations into screen space
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(position_in.xy, texture_color[0]/5.f, 1.0);
}