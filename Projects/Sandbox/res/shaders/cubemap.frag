#version 460 core

layout (location = 0) out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	vec3 envColor = textureLod(skybox, TexCoords, 0.0).rgb;

	FragColor = vec4(envColor, 1.0);
}