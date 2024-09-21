#version 430

// INPUT
// - nothing ! this is purely procedural geometry

// UNIFORMS
layout(location = 0) uniform mat4 uModelMatrix;
layout(location = 1) uniform mat4 uViewMatrix;
layout(location = 2) uniform mat4 uProjectionMatrix;
layout(location = 3) uniform int uNbVertices;
layout(location = 4) uniform int uNbInstances;
layout(location = 5) uniform float uTime;
// blue noise generation function
float noise(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// MAIN PROGRAM
void main()
{
    gl_PointSize = 5.f;
    // PROCEDURAL GENERATION:
    // - gl_VertexID and gl_InstanceID are predefined GLSL variables
    // - we can use them to specify data for each vertex and each instance
    // - let first create a parameter space (u,v) in [0.0;1.0]
    float u = float(gl_VertexID) / float(uNbVertices - 1);
    float v = float(gl_InstanceID) / float(uNbInstances - 1);
    // - now remap to [-0.5;0.5] to center the grid
    vec2 position = vec2(u, v) - 0.5;

    // - add turbulence on the position
    float vertexRand = 0.5 * sin(uTime / 100.0) + 0.5;
    float instanceRand = 0.5 * cos(uTime / 210.0) + 0.5;
    float turbulence = noise(vec2(position.x * vertexRand, position.y * instanceRand));
    position.y += (turbulence / 15.0); // tune the height of turbulence
    
    // - write position
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(position, 0.0, 1.0);
}