#version 430

// INPUT
// - the currently bounded vertex array (VAO) contains a VBO of 2D data (positions)
// - variable is prefixed by "in"
// - its "location index" MUST be the same value when using vertexAttribPointer() and enableVertexAttribArray() during VAO definition
layout(location = 1) in vec2 position_in;
layout(location = 2) uniform float time;
layout(location = 3) out vec2 position_out;
layout(location = 4) uniform mat4 model;

float fmod(float a, float b) {
	while (a<0) {
		a += b;
	}
	while (a>=b) {
		a -= b;
	}
	return a;
}

float cycle(float a, float b) {
	if (a<0) {
		return 0;
	}
	if (fmod((a/b), 2.f) < 1.f) {
		return fmod(a, b);
	}
	return b - fmod(a, b);
}

// MAIN PROGRAM
void main()
{
	position_out = position_in;

	// "gl_PointSize" is a predined variable in GLSL to fix the size of primitives of type "GL_POINTS"
	// it wiil be used by the "rasterizer" to generate points of given size in "pixels"
	gl_PointSize = cycle(time*15.f, 50.f);

	// MANDATORY
	// - a vertex shader MUST write the value of the predefined variable " (GLSL langage)"
	// - this value represent a position in NDC space (normalized device coordintes), i.e the cube [-1.0;1.0]x[-1;1.0]x[-1;1.0]
	gl_Position = model*vec4(position_in, 0.0, 1.0);
}