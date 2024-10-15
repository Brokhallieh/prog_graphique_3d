#version 430

// INPUT
layout(location = 1) in vec3 position_in;
layout(location = 2) in vec2 textureCoord_in;

//UNIFORM 
layout(location = 0) uniform mat4 uModelMatrix;
layout(location = 1) uniform mat4 uViewMatrix;
layout(location = 2) uniform mat4 uProjectionMatrix;
// OUTPUT
out vec2 v_textureCoord;

// MAIN PROGRAM
void main()
{
	// Send texture coords to fragment shader via the graphics pipeline
	v_textureCoord = textureCoord_in;

	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(position_in, 1.0);
}