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

//UNIFORM LOCATION
layout(location = 4) uniform vec3 uLightIntensity;
layout(location = 5) uniform vec3 uLightPos;
layout(location = 6) uniform vec3 uKa;
layout(location = 7) uniform vec3 uKd;
layout(location = 8) uniform vec3 uKs;
layout(location = 9) uniform float uNs;
layout(location = 10) uniform bool isPlanet;

// MAIN PROGRAM
void main()
{
	vec3 naturalColor = texture(uColor, texCoord).xyz;

	vec3 normal = normalize(v_norm);
	if (gl_FrontFacing == false) normal = -normal;

	vec3 lightDir = normalize(uLightPos - v_pos);

	vec3 Ia = uLightIntensity * uKa;

	float diffuseTerm = max(0.f, dot(normal, lightDir));

	vec3 Kd = uKd;
	if (isPlanet)
	{
		Kd = naturalColor;
	}
	vec3 Id = uLightIntensity * Kd * vec3(diffuseTerm);

	Id /= PI;

	vec3 Is = vec3(0.f);
	if (diffuseTerm > 0.f)
	{
		vec3 viewDir = normalize(-v_pos.xyz);
		vec3 halfDir = normalize(viewDir + lightDir);
		float specularTerm = max(0.f, pow(dot(normal, halfDir), uNs));
		Is = uLightIntensity * uKs * vec3(specularTerm);
		Is /= (uNs + 2.f) / (2.f * PI);
	}

	vec3 finalColor = (0.3 * Ia) + (0.3 * Id) + (0.3 * Is);

	oFragmentColor = vec4(mix(vec3(0.f), naturalColor, finalColor), 1.f);
}