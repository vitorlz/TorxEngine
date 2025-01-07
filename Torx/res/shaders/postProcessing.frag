#version 460 core

out vec4 FragColor;

in vec2  TexCoords;

uniform sampler2D screenQuadTexture;
uniform sampler2D bloomBlurTexture;

// SSR
uniform sampler2D ssrTexture;
uniform sampler2D ssrTextureBlur;
uniform sampler2D gPosition;
uniform sampler2D gRMA;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform float ssrMaxBlurDistance;
uniform float ssrSpecularBias;

uniform vec3 camPos;

uniform bool showNormals;
uniform bool reinhard;
uniform bool uncharted2;
uniform bool ACES;
uniform float exposure;
uniform bool bloom;
uniform bool worldPosDebug;
uniform bool albedoDebug;
uniform bool roughnessDebug;
uniform bool metallicDebug;


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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{

	
	vec3 FragPos = texture(gPosition, TexCoords).xyz;
	vec3 RMA = texture(gRMA, TexCoords).rgb;
	vec3 V = normalize(camPos - FragPos);
	vec3 N = texture(gNormal, TexCoords).rgb;
	vec3 albedo = texture(gAlbedo, TexCoords).rgb;

	float roughness = RMA.r;
	float metallic = RMA.g;

	const float gamma = 2.2;
	vec3 color = vec3(texture(screenQuadTexture, TexCoords));
	vec3 bloomColor;

	if (bloom)
	{
		vec3 bloomColor = vec3(texture(bloomBlurTexture, TexCoords));
		color += bloomColor;
	}
	
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 F = fresnelSchlick(max(dot(N, V), 0.0), F0);

	vec3 kS = F;

	vec3 ssrOriginal = texture(ssrTexture, TexCoords).rgb;

	vec3 ssrBlurred = texture(ssrTextureBlur, TexCoords).rgb;
	
	vec3 ssr = mix(ssrOriginal, ssrBlurred, clamp(sqrt(roughness * 1.1) , 0,1)) * max(0, (1 - roughness * 2)) * ssrSpecularBias;
	
	color += kS * mix(ssr, vec3(0.0),  smoothstep(0.0, 1.0, dot(V,reflect(-V, N))));

	if (!showNormals && !worldPosDebug && !albedoDebug && !roughnessDebug && !metallicDebug) {
		
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