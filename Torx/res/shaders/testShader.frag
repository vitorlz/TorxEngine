#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

struct Material {
	sampler2D texture_diffuse1;

	sampler2D texture_specular1;

	sampler2D texture_normal1;

	sampler2D texture_emission1;

	sampler2D texture_height1;

	float shininess;
};

uniform Material material;

void main()
{
	
	float gamma = 2.2;

	vec3 color = texture(material.texture_diffuse1, TexCoord).rgb;

	color = pow(color, vec3(1/gamma));

    FragColor = vec4(color * 2, 1.0);
}