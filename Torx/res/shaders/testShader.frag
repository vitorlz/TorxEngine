#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

struct Material {
	sampler2D texture_albedo1;
};

uniform Material material;

void main()
{
	
	float color = texture(material.texture_albedo1, TexCoord).r;

    FragColor = vec4(vec3(color), 1.0);
}