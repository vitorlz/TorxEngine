#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main()
{
	
	// The geometry shader will be the shader responsible for transforming all world-space vertices to the 6 different light spaces. 
	// Therefore, the vertex shader simply transforms vertices to world-space and directs them to the geometry shader:

	gl_Position = model * vec4(aPos, 1.0);
}