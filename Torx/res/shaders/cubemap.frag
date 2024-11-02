#version 460 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCubeArray skybox;

void main()
{
	FragColor = texture(skybox, vec4(TexCoords, 0.0));
}