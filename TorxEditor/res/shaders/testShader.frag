	#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

struct Material {
	sampler2D texture_albedo1;
};

uniform Material material;

void main()
{
	
	vec3 color = texture(material.texture_albedo1, TexCoord).rgb;

    FragColor = vec4(color, 1.0);
}