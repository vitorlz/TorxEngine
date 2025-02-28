#version 460 core

uniform ivec3 gridDimensions;  // dimensions of the voxel grid
uniform sampler3D voxelTexture; // 3d texture containing voxel colors

out vec3 gridCoord;
out vec4 voxelColor;
out vec3 worldPos;
out mat4 projView;
out vec3 camPosGeom;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 camPos;

void main()
{
    // calculate grid coordinates using gl_VertexID
    int z = gl_VertexID / (gridDimensions.x * gridDimensions.y);
    int rem = gl_VertexID % (gridDimensions.x * gridDimensions.y);
    int y = rem / gridDimensions.x;
    int x = rem % gridDimensions.x;

    gridCoord = vec3(x, y, z);

    vec3 texCoord = gridCoord / vec3(gridDimensions);

    // sample the color from the 3D texture
    voxelColor = textureLod(voxelTexture, vec3(texCoord.x, texCoord.y, texCoord.z), 0);

    // center the voxel position in the world space
    worldPos = ((gridCoord - vec3(gridDimensions) * 0.5 + 0.5) * 0.5) - camPos;
    
    projView = projection * view;

    gl_Position;
}