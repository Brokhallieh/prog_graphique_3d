#version 430
precision highp float;

#define PI 3.141592653589793f

// INPUT
in vec3 v_pos;
in vec2 texCoord;
in vec3 v_norm;

// OUTPUT
out vec4 oFragmentColor;

// UNIFORM BINDING
layout(binding = 0) uniform sampler2D uColor;
layout(binding = 1) uniform sampler2D uColorClouds;
layout(binding = 2) uniform sampler2D uColorNight;

//UNIFORM LOCATION
layout(location = 4) uniform vec3 uLightIntensity;
layout(location = 5) uniform vec3 uLightPos;
layout(location = 6) uniform vec3 uKa;
layout(location = 7) uniform vec3 uKs;
layout(location = 8) uniform float uNs;

// MAIN PROGRAM
void main()
{
	vec3 colorDay = texture(uColor, texCoord).xyz, colorClouds = texture(uColorClouds, texCoord).xyz, colorNight = texture(uColorNight, texCoord).xyz;

	vec3 normal = normalize(v_norm);
	if (gl_FrontFacing == false) normal = -normal;

	vec3 lightDir = normalize(uLightPos - v_pos);

	vec3 Ia = uLightIntensity * uKa;

	// Diffuse lighting : lambert BRDF
	// Get the angle between the normal of the fragment and the light direction to the fragment
	float diffuseTerm = max(0.f, dot(normal, lightDir)); // "max" is used to avoid "back" lighting (when light is behind the object)
	vec3 Id = uLightIntensity * vec3(1.f) * vec3(diffuseTerm);

	Id /= PI; // normalization of the diffuse BRDF (for energy conservation)

	// Specular lighting : phong BRDF
	vec3 Is = vec3(0.f);
	if (diffuseTerm > 0.f)
	{
		vec3 viewDir = normalize(-v_pos.xyz); // "view direction" from current vertex position => because, in View space, "dir = vec3(0.0, 0.0, 0.0) - p"
		vec3 halfDir = normalize(viewDir + lightDir); // half-vector between view and light vectors
		float specularTerm = max(0.f, pow(dot(normal, halfDir), uNs)); // "Ns" control the size of the specular highlight
		Is = uLightIntensity * uKs * vec3(specularTerm);
		Is /= (uNs + 2.f) / (2.f * PI); // normalization of the specular BRDF (for energy conservation)
	}
	vec3 texEarth = mix(colorNight, colorDay, (0.3 * Ia) + (0.3 * Id) + (0.3 * Is));
	vec3 finalColor = mix(texEarth, (0.3 * Ia) + (0.3 * Id) + (0.3 * Is) + 0.08f, colorClouds);

	oFragmentColor = vec4(finalColor, 1.f);
}