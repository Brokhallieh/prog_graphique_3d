#version 430

layout(location = 1) in vec3 position_in;
layout(location = 2) in vec3 normal_in;
layout(location = 3) in vec2 texCoord_in;
layout(location = 4) in vec3 tangents_in;

layout (location = 0) uniform mat4 modelMatrix;
layout (location = 1) uniform mat4 projectionMatrix;
layout (location = 2) uniform mat4 viewMatrix;
layout (location = 3) uniform mat3 normalMatrix;
layout (location = 19) uniform float tillingFactor;
layout (location = 20) uniform mat4 shadowViewMatrix;
layout (location = 21) uniform mat4 shadowProjectionMatrix;

out vec3 w_pos;
out vec3 w_normal;
out vec2 texCoord;
out mat3 TBN;
out vec4 ShCoord;

void main()
{
	vec4 P4 = modelMatrix * vec4(position_in, 1);
    gl_Position = projectionMatrix * viewMatrix * P4;
	
    w_pos = P4.xyz;
	w_normal = normalMatrix * normal_in;
    
    texCoord = texCoord_in * tillingFactor;

    // TBN matrix
	vec3 T = normalize(normalMatrix * tangents_in);
	vec3 N = normalize(w_normal);
	vec3 B = cross(N, T); // bitangents
   	TBN = mat3(T, B, N); // TBN matrix transform from tangent space to world space 

	// Shadow coords
    ShCoord = shadowProjectionMatrix * shadowViewMatrix * P4;
}