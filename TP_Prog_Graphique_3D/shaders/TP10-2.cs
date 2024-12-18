#version 430

precision highp float;

// each bloc of threads: 2D blocs of 32x32 threads
layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (location = 0) uniform int uMaxNbIterations;
layout (location = 1) uniform vec3 uZoom;
layout(binding = 0, rgba32f) writeonly uniform image2D imgOutput;

// Color palette
vec3 palette(float t, vec3 a, vec3 b, vec3 c, vec3 d)
{
    return a + b*cos( 6.28318*(c*t+d) );
}

uint mandelbrot(vec2 c, inout bool escaped)
{
	vec2 z = vec2(0.0, 0.0);
    int iterations = 0;
    
    for (int i = 0; i < uMaxNbIterations; ++i)
    {
        iterations = i;
        z = vec2(z.x * z.x - z.y * z.y + c.x, 2.f * z.x * z.y + c.y);

        if (length(z) > 2.f)
        {
            escaped = true;
            break;
        }
    }

	return iterations;
}

void main()
{
	// Get pixel position
	ivec2 pixelPosition = ivec2(gl_GlobalInvocationID.xy);
    // Get image size
	ivec2 imSize = imageSize(imgOutput);
    // Check bounds for unneeded threads (and memory access)
	if (pixelPosition.x > imSize.x || pixelPosition.y > imSize.y) return;

    // define coords
    vec2 zoomCenter = vec2(uZoom.xy);
    float zoomSize = uZoom.z;
    vec2 uv = pixelPosition / vec2(imSize);  // in [0;1]
    vec2 coords = zoomCenter + (uv * 4.f - vec2(2.f)) * (zoomSize / 4.f);

    bool escaped = false;
    uint iterations = mandelbrot(coords, escaped);
    
    // vec4 color = vec4(float(iterations)/(uMaxNbIterations/3.f), float(iterations)/uMaxNbIterations, float(iterations)/uMaxNbIterations, 1.f);
    vec4 color = escaped ? vec4(palette(float(iterations)/float(uMaxNbIterations), vec3(0.0),vec3(0.59,0.55,0.75),vec3(0.1, 0.2, 0.3),vec3(0.75)),1.0) : vec4(vec3(0.85, 0.99, 1.0), 1.0);

    // Write data to texture
    imageStore(imgOutput, pixelPosition, color);
}
