#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform bool horizontal;
uniform float kernelSize;
uniform float weight[100]; 

void main() 
{
	vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of a single texel
	vec3 result = texture(image, TexCoords).rgb * weight[0]; // current fragment's contribution
	if(horizontal)
	{
		for(int i = 1; i <= kernelSize; i += 1) 
		{
			result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb *	weight[i];
			result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb *	weight[i];
		}
	}
	else {
		for(int i = 1; i < 5; i++) 
		{
			result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb *	weight[i];
			result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb *	weight[i];
		}
	}

	FragColor = vec4(result, 1.0);
}