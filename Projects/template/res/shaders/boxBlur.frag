#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorTexture;

// control how blurry the result is
int size = 4;
float separation = 5;

void main()
{
	vec2 texSize = textureSize(colorTexture, 0).xy;
	vec2 texCoord = gl_FragCoord.xy / texSize;

	separation = max(separation, 1);

	for (int i = -size; i <= size; ++i) 
	{
		for (int j = -size; j <= size; ++j) 
		{
		   FragColor += texture(colorTexture, clamp((gl_FragCoord.xy + (vec2(i, j) * separation)), vec2(0.0), texSize - 0.01)/texSize) ; 
		}
	}

	FragColor /= pow(size * 2 + 1, 2);
}