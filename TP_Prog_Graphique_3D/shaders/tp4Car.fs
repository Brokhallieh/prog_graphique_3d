#version 430
precision highp float;

#define M_PI 3.14159265358979
// IN
in vec3 v_pos;
in vec3 v_norm;
in vec4 worldPos;
in vec3 worldNormal;

// UNIFORM
layout(location = 4) uniform vec3 uLightIntensity;
layout(location = 5) uniform vec3 uLightPos;
layout(location = 6) uniform vec3 uKa;
layout(location = 7) uniform vec3 uKd;
layout(location = 8) uniform vec3 uKs;
layout(location = 9) uniform float uNs;
layout(location = 10)uniform float coeffRefl;
layout(location = 11) uniform float uOpacity; 
layout(location = 12) uniform vec4 CameraPosWorld; 
layout(binding = 0) uniform samplerCube uSampler;


// OUTPUT
out vec4 oFragmentColor;

// MAIN PROGRAM
void main()
{
	vec3 normal = normalize(v_norm);
	if (gl_FrontFacing == false) normal = -normal;

	// Get the vector between the fragment position and the light position
	vec3 lightDir = normalize(-uLightPos-v_pos); // Point light
	vec3 viewDir = normalize(-v_pos.xyz); // "view direction" from current vertex position => because, in View space, "dir = vec3(0.0, 0.0, 0.0) - p"

	// Ambient lighting : approximation...
	vec3 Ia = uLightIntensity * uKa;

	// Diffuse lighting : lambert BRDF
	// Get the angle between the normal of the fragment and the light direction to the fragment
	float diffuseTerm = max(0.f, dot(normal, lightDir)); // "max" is used to avoid "back" lighting (when light is behind the object)
	vec3 Id = uLightIntensity * uKd * vec3(diffuseTerm);
	Id = Id / M_PI; // normalization of the diffuse BRDF (for energy conservation)

	// Specular lighting : phong BRDF
	vec3 Is = vec3(0.0);
	if (diffuseTerm > 0.0)
	{
		
		vec3 halfDir = normalize(viewDir + lightDir); // half-vector between view and light vectors
		float specularTerm = max(0.0, pow(dot(normal, halfDir), uNs)); // "Ns" control the size of the specular highlight
		Is = uLightIntensity * uKs * vec3(specularTerm);
		Is /= (uNs + 2.f) / (2.f * M_PI); // normalization of the specular BRDF (for energy conservation)
	}

	vec3 I = normalize(worldPos.xyz - CameraPosWorld.xyz);
	vec3 R = reflect(I, normalize(worldNormal));
	vec3 envColor = texture(uSampler,R).rgb;

	// Reflected intensity (i.e final color) from additif model
	vec3 finalColor = mix((0.3 * Ia) + (0.3 * Id) + (0.3 * Is),envColor,coeffRefl);

	oFragmentColor = vec4(finalColor, uOpacity);
}