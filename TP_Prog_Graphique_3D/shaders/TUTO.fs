#version 430
precision highp float;

#define M_PI 3.14159265358979
// CONSTANT
const vec3 gravelColor = vec3(0.29, 0.24, 0.22);
const vec3 dirtColor = vec3(0.45, 0.34, 0.19);
const vec3 grassColor = vec3(0.22, 0.63, 0.28);
const vec3 rockColor = vec3(0.33, 0.31, 0.31);
const vec3 snowColor = vec3(0.90, 0.90, 0.95);

// INPUT
layout(location = 0) in vec3 normal;
layout(location = 1) in vec3 uCoords;

// UNIFORM
layout(location = 13) uniform vec3 uLightPos;
layout(location = 14) uniform float uLightIntensity;

// OUTPUT
out vec4 oFragmentColor;

// MAIN PROGRAM
void main()
{
	float height = uCoords[1];
	// COLOR
	vec3 gravelDirt = mix(gravelColor, dirtColor, smoothstep(-0.5, 0.02, height));
	vec3 dirtGrass = mix(gravelDirt, grassColor, smoothstep(0.03, 0.06, height));
	vec3 grassRock = mix(dirtGrass, rockColor, smoothstep(0.09, 0.10, height));
	vec3 rockSnow = mix(grassRock, snowColor, smoothstep(0.14, 0.15, height));
	vec3 color = rockSnow.rgb;

	vec3 uNormal = normal;

	if (gl_FrontFacing == false) uNormal = -uNormal;

	// Get the vector between the fragment position and the light position
	vec3 lightDir = normalize(uLightPos - uCoords); // Point light

	// Diffuse lighting : lambert BRDF
	// Get the angle between the normal of the fragment and the light direction to the fragment
	float diffuseTerm = max(0.f, dot(uNormal, lightDir)); // "max" is used to avoid "back" lighting (when light is behind the object)
	vec3 Id = uLightIntensity * color * vec3(diffuseTerm);
	Id = Id / M_PI; // normalization of the diffuse BRDF (for energy conservation)

	oFragmentColor = vec4(Id, 1.f);
}