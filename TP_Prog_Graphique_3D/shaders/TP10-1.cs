#version 430

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0, rgba32f) writeonly uniform image2D imgOutput;

void main()
{
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    vec4 value = vec4(0.f, 0.f, 0.f, 1.f);
    value.xy = vec2(texelCoord) / (gl_NumWorkGroups.xy * gl_WorkGroupSize.xy);
	
    imageStore(imgOutput, texelCoord, value);
}
