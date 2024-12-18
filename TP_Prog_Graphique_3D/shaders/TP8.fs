#version 430
precision highp float;

#define M_PI 3.14159265358979

in vec3 w_pos;
in vec3 w_normal;
in vec2 texCoord;
in mat3 TBN;
in vec4 ShCoord;

out vec4 frag_out;

layout (location = 4) uniform vec3 light_pos[4];
layout (location = 8) uniform vec3 light_col[4];
layout (location = 12) uniform bool attenuation;
layout (location = 13) uniform bool gammaCorrection;
layout (location = 14) uniform bool toneMapping;
layout (location = 15) uniform float exposure;
layout (location = 16) uniform bool useNormalMap;
layout (location = 17) uniform bool useShadowMap;
layout (location = 18) uniform float darkness;
layout (location = 22) uniform float biasMax;
layout (binding = 0) uniform sampler2D uColor;
layout (binding = 1) uniform sampler2D normalMap;
layout (binding = 2) uniform sampler2D texDepth;

float ShadowCalculation(vec3 normal, vec3 lightDir)
{
	// Transform shadowCoords in NDC and from [-1,1] to [0,1]
    vec3 projCoords = ShCoord.xyz/ShCoord.w * 0.5f + 0.5f;
	
	// to correct shadow acne
	float bias = max(biasMax * (1.f - dot(normal, lightDir)), 0.005f);
	projCoords.z -= bias;

	// Get the current depth
	float currentDepth = projCoords.z;

	// force to have no shadow if the z depth value is greater than one (outside the far plane of the light's orthographic frustrum) 
	if (currentDepth >= 0.99f) return 1.f;
	
	// Get the shadow
	//==========================

	// 1.2 Without PCF
	float closestDepth = texture(texDepth, projCoords.xy).r;
	float shadow = currentDepth > closestDepth  ? darkness : 0.f;

	// 2 With PCF
	// float shadow = 0.f;
	// vec2 texelSize = 1.f / vec2(textureSize(texDepth, 0));
	
	// for(int x = -2; x <= 2; ++x)
    // {
    //     for(int y = -2; y <= 2; ++y)
    //     {
    //         float pcfDepth = texture(texDepth, projCoords.xy + vec2(x, y) * texelSize).r; 
    //         shadow += currentDepth > pcfDepth ? darkness : 0.f;
    //     }    
    // }
	// shadow /= 25.f;

	return shadow;
}

void main()
{
	vec3 diffuseColor;
	if (gammaCorrection)
    {
		float gamma = 2.2f;
		diffuseColor = pow(texture(uColor, texCoord).rgb, vec3(gamma)); // to avoid double gamma correction for textures
	}
	else diffuseColor = texture(uColor, texCoord).rgb;

	vec3 normal = normalize(w_normal);

    if (useNormalMap)
	{	
		normal = texture(normalMap, texCoord).xyz;
		normal = normal * 2.0 - 1.0; // remap from [0,1] to [-1,1]
		normal = normalize(TBN * normal); // go back from tangent space to world space  
	}
    
    if (!gl_FrontFacing) normal = -normal;

	vec3 hdrColor = vec3(0.f);
    vec3 lightDir;
	for (int i = 0; i < 4; ++i)
	{
		lightDir = normalize(light_pos[i] - w_pos);

		// Diffuse lighting : lambert BRDF
		float diffuseTerm = max(0.f, dot(normal, lightDir));
		vec3 Id = light_col[i] * diffuseColor * vec3(diffuseTerm);
		Id = Id / M_PI; // normalization of the diffuse BRDF (for energy conservation)

		if (attenuation)
		{
			float distance = length(w_pos - light_pos[i]);
			// linear or quadratic attenuation depend on gamma correction
			Id *= gammaCorrection ? 1.f / (distance * distance) : 1.f / distance;
		}

		hdrColor += Id;
	}

    if (useShadowMap)
    {
        lightDir = normalize(light_pos[0] - w_pos); // Point light
        float shadow = ShadowCalculation(normal, lightDir);
        hdrColor = ((1.f - shadow) * hdrColor);
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
}