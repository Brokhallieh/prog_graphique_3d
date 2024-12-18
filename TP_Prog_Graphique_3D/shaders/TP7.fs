#version 430
precision highp float;

#define M_PI 3.14159265358979

in vec3 w_pos;
in vec3 w_normal;

out vec4 frag_out;

layout (location = 4) uniform vec3 albedo;
layout (location = 5) uniform vec3 light_pos[4];
layout (location = 9) uniform vec3 light_col[4];
layout (location = 13) uniform bool attenuation;
layout (location = 14) uniform bool gammaCorrection;
layout (location = 15) uniform bool toneMapping;
layout (location = 16) uniform float exposure;

void main()
{
	vec3 normal = normalize(w_normal);
	if (!gl_FrontFacing) normal = -normal;

	vec3 hdrColor = vec3(0.f);

	for (int i = 0; i < 4; ++i)
	{
		// Get the vector between the fragment position and the light position
		vec3 lightDir = normalize(light_pos[i] - w_pos); // Point light

		// Diffuse lighting : lambert BRDF
		// Get the angle between the normal of the fragment and the light direction to the fragment
		float diffuseTerm = max(0.f, dot(normal, lightDir));  // "max" is used to avoid "back" lighting (when light is behind the object)
		vec3 Id = light_col[i] * albedo * vec3(diffuseTerm);
		Id = Id / M_PI; // normalization of the diffuse BRDF (for energy conservation)

		if (attenuation)
		{
			// attenuation (use quadratic as we have gamma correction)
			float distance = length(w_pos - light_pos[i]);
			Id *= gammaCorrection ? 1.f / (distance * distance) : 1.f / distance; // linear or quadratic attenuation depend on gamma correction
		}

		hdrColor += Id;
	}

	// tone mapping
    vec3 finalColor = toneMapping? vec3(1.f) - exp(-hdrColor * exposure) : hdrColor;

    if (gammaCorrection)
    {
        float gamma = 2.2f;

        // gamma correction 
		finalColor = pow(finalColor, vec3(1.f / gamma));
    }
	
    frag_out = vec4(finalColor, 1.f);

	// DEBUG
    // frag_out = vec4(albedo, 1.f);
}