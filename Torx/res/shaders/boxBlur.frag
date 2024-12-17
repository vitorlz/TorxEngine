#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorTexture;

// control how blurry the result is
int size = 8;
float separation = 1;

void main()
{
	vec2 texSize = textureSize(colorTexture, 0).xy;
	vec2 texCoord = gl_FragCoord.xy / texSize;

	if(size <= 0) 
	{
		FragColor = texture(colorTexture, texCoord);
		return;
	}

	separation = max(separation, 1);

	for (int i = -size; i <= size; ++i) 
	{
		for (int j = -size; j <= size; ++j) 
		{
		   FragColor += texture(colorTexture, (gl_FragCoord.xy + (vec2(i, j) * separation)) / texSize); 
		}
	}

	FragColor /= pow(size * 2 + 1, 2);
}