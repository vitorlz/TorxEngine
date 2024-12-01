#version 460 core

in vec3 voxelColor;  // Input color from the vertex shader

layout (location = 0) out vec4 FragColor; // Final color output

void main()
{  
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Set the color with full alpha
}