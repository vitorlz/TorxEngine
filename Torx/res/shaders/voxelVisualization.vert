#version 460 core

uniform ivec3 gridDimensions;  // Dimensions of the voxel grid
uniform mat4 view;             // View matrix
uniform mat4 projection;       // Projection matrix
uniform sampler3D voxelTexture; // 3D texture containing voxel colors

out vec3 gridCoord;
out vec3 voxelColor;  // Color output to fragment shader

void main()
{
    // Calculate grid coordinates using gl_VertexID
    int z = gl_VertexID / (gridDimensions.x * gridDimensions.y);
    int rem = gl_VertexID % (gridDimensions.x * gridDimensions.y);
    int y = rem / gridDimensions.x;
    int x = rem % gridDimensions.x;

    gridCoord = vec3(x, y, z);

    // Calculate texture coordinates (normalized)
    vec3 texCoord = gridCoord / vec3(gridDimensions);

    // Sample the color from the 3D texture
    voxelColor = texture(voxelTexture, texCoord).rgb;

    // Center the voxel position in the world space
    vec3 worldPos = (gridCoord - vec3(gridDimensions) * 0.5 + 0.5);
    
    gl_Position = projection * view * vec4(worldPos, 1.0);
}