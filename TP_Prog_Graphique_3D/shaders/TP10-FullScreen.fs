#version 430

// INPUT
in vec2 texCoord;

// OUTPUT
out vec4 oColor;

layout (binding = 0) uniform sampler2D tex;

void main()
{
	oColor = texture(tex, texCoord);
}