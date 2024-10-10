#version 430

// INPUT
layout(location = 1) in vec3 position_in;

// OUTPUT
out vec3 position;

// UNIFORM
layout(location = 0) uniform mat4 uModelMatrix;
layout(location = 1) uniform mat4 uViewMatrix;
layout(location = 2) uniform mat4 uProjectionMatrix;

// MAIN PROGRAM
void main()
{
	mat4 viewNoTranslation = uViewMatrix;
    viewNoTranslation[3][0] = 0.0;
    viewNoTranslation[3][1] = 0.0;
    viewNoTranslation[3][2] = 0.0;
	position = position_in;
	// - write position with matrix transformations into screen space
	vec4 pos = uProjectionMatrix * viewNoTranslation * uModelMatrix * vec4(position_in, 1.);
	gl_Position = vec4(pos.xy, pos.w, pos.w);
}