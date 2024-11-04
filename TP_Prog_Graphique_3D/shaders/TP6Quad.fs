#version 430
precision highp float;

#define M_PI 3.14159265358979

// UNIFORM
// Material (BRDF: bidirectional reflectance distribution function)
layout(binding = 0) uniform sampler2D position;
layout(binding = 1) uniform sampler2D normal;
layout(binding = 2) uniform sampler2D albedo;
// Light (Point light)
layout(location = 5) uniform float lightIntensity;
layout(location = 6) uniform int nb_lum;
uniform vec3 pos_lum[100];
uniform vec3 col_lum[100];

// OUTPUT
out vec4 oFragmentColor;

// MAIN PROGRAM
void main()
{
	ivec2 coord = ivec2(gl_FragCoord.xy);
	vec3 p = texelFetch(position, coord, 0).xyz;
	vec3 n = texelFetch(normal, coord, 0).xyz;
	vec3 a = texelFetch(albedo, coord, 0).xyz;

	// --------------------------------------
	// Lighting and shading: PER-FRAGMENT
	// - here, we "retrieve" mandatory information from the vertex shader (i.e. "position" and "normal")
	// --------------------------------------
	
	n = normalize(n); // interpolated normal direction from current interpolated position in View space
	
	vec3 color = vec3(0);

	for (int i = 0; i < nb_lum; ++i)
	{
		// Reflected diffuse intensity
		vec3 lightDir = normalize(pos_lum[i] - p); // "light direction" from current interpolated position in View space
		float diffuseTerm = max(0.f, dot(n, lightDir)); // "max" is used to avoir "back" lighting (when light is behind the object)
		vec3 Id = (col_lum[i] * lightIntensity) * a * vec3(diffuseTerm);
		Id = Id / M_PI; // normalization of the diffuse BRDF (for energy conservation)
		
		// Reflected intensity accumulation (i.e final color)
		color += Id;
	}
	// --------------------------------------
	
	oFragmentColor = vec4(color, 1.f);
}