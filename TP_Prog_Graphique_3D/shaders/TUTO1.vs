#version 430

// INPUT
// - the currently bounded vertex array (VAO) contains a VBO of 2D data (positions)
// - variable is prefixed by "in"
// - its "location index" MUST be the same value when using vertexAttribPointer() and enableVertexAttribArray() during VAO definition
layout(location = 0) in vec3 position_in;
layout(location = 1) uniform mat4 model;
layout(location = 2) uniform mat4 view;
layout(location = 3) uniform mat4 proj;

// UNIFORM

// MAIN PROGRAM
void main()
{

    // MANDATORY
	// - a vertex shader MUST write the value of the predefined variable " (GLSL langage)"
	// - this value represent a position in NDC space (normalized device coordintes), i.e the cube [-1.0;1.0]x[-1;1.0]x[-1;1.0]
	gl_Position = proj*view*model*vec4(position_in, 1.0);
}