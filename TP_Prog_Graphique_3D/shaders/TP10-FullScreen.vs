#version 430

// OUTPUT
// Texture coordinates
out vec2 texCoord;

void main()
{
	// Compute vertex position between [-1;1]
	float x = -1.f + float((gl_VertexID & 1) << 2); // If VertexID == 1 then x = 3 else x == -1
	float y = -1.f + float((gl_VertexID & 2) << 1); // If VertexID == 2 then y = 3 else y == -1

	// Compute texture coordinates between [0;1] (-1 * 0.5 + 0.5 = 0 and 1 * 0.5 + 0.5 = 1)
	texCoord.x = x * 0.5 + 0.5;
	texCoord.y = y * 0.5 + 0.5;

	// Send position to clip space
	gl_Position = vec4(x, y, 0.f, 1.f);
}