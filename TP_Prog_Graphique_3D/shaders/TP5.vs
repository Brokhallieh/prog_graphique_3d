//--------------------------------------------------------------------------------------------------------
// VERTEX SHADER (GLSL language)
//
// - render a full-screen quad
// MEGA-TRICK : with only 1 triangle whose size is "2 times" the classical size [-1;-1]x[1;1] where GL points lie 
//            - The point positions are procedurally generated in the vertex shader (with gl_VertexID)
//            - At the "clipping" stage, after vertex shader, before rasterization, new points
//              are generated at the corner of the "unit cube" [-1;-1;-1]x[1;1;1] in clip space,
//              => the geometry of the triangle is clipped, and the rasterizer generate all fragments inside the viewport
//              This is the way "shadertoy" is working	
// => NO VAO, NO VBO, ONLY 1 triangle : glDrawArrays(GL_TRIANGLES, 0, 3);
// https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/
//--------------------------------------------------------------------------------------------------------
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