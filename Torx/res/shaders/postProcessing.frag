#version 460 core

out vec4 FragColor;

in vec2  TexCoords;

uniform sampler2D screenQuadTexture;
uniform sampler2D bloomBlurTexture;

uniform bool showNormals;
uniform bool reinhard;
uniform bool uncharted2;
uniform bool ACES;
uniform float exposure;
uniform bool bloom;
uniform bool worldPosDebug;

// testing some tone mapping algorithms

vec3 uncharted2_tonemap_partial(vec3 x)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 uncharted2_filmic(vec3 v)
{
    float exposure_bias = exposure;
    vec3 curr = uncharted2_tonemap_partial(v * exposure_bias);

    vec3 W = vec3(11.2f);
    vec3 white_scale = vec3(1.0f) / uncharted2_tonemap_partial(W);
    return curr * white_scale;
}

vec3 aces_approx(vec3 v)
{
    v *= exposure * 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

vec3 reinhardSimple(vec3 v) {
	vec3 mapped = vec3(1.0) - exp(-v * exposure);
	return mapped;
}

void main()
{

	const float gamma = 2.2;
	vec3 color = vec3(texture(screenQuadTexture, TexCoords));
	vec3 bloomColor;

	if (bloom)
	{
		vec3 bloomColor = vec3(texture(bloomBlurTexture, TexCoords));
		color += bloomColor;
	}

	if (!showNormals && !worldPosDebug) {
		
		if(reinhard)
		{
			color = mix(color, reinhardSimple(color), 1);
		}
		else if (uncharted2)
		{
			color = mix(color, uncharted2_filmic(color), 1);
		}
		else if (ACES)
		{
			color = mix(color, aces_approx(color), 1);
		}

		color = pow(color, vec3(1.0 / gamma));
	}

	FragColor = vec4(color, 1.0);

}