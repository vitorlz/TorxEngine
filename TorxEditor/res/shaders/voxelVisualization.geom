#version 460 core

// This shader expands points into cubes. It is definitely not the best way of visualizing the voxels as it is extremely performance intensive.

layout(points) in;               
layout(triangle_strip, max_vertices = 24) out; 

in vec3 gridCoord[];           
in vec4 voxelColor[];
in vec3 worldPos[];

in mat4 projView[];      

out vec4 fragColor;             

const float halfSize = 0.2499; 

void main()
{
    vec3 vertices[8] = vec3[](
        vec3(-halfSize, -halfSize, -halfSize),
        vec3( halfSize, -halfSize, -halfSize),
        vec3(-halfSize,  halfSize, -halfSize),
        vec3( halfSize,  halfSize, -halfSize),
        vec3(-halfSize, -halfSize,  halfSize),
        vec3( halfSize, -halfSize,  halfSize),
        vec3(-halfSize,  halfSize,  halfSize),
        vec3( halfSize,  halfSize,  halfSize)
    );

    int indices[24] = int[](
        0, 1, 2, 3,  
        4, 5, 6, 7,  
        0, 2, 4, 6,  
        1, 3, 5, 7,  
        0, 1, 4, 5,  
        2, 3, 6, 7   
    );

    // emit vertices for the cube
    for (int i = 0; i < 24; ++i)
    {
        vec3 position = worldPos[0] + vertices[indices[i]]; 
        gl_Position = projView[0] * vec4(position, 1.0);
        fragColor = voxelColor[0]; 
        EmitVertex();
    }
    EndPrimitive();
}