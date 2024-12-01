#version 460 core

struct Material 
{
	sampler2D texture_albedo1;

	sampler2D texture_normal1;

	sampler2D texture_rma1;

	sampler2D texture_emission1;
};

uniform Material material;
layout(rgba8, binding = 0) uniform image3D texture3D;

struct Light
{
	vec4 type;
	vec4 position;
	vec4 color;
	vec4 radius;

	// for spotlight
	vec4 direction;
	vec4 innerCutoff;
	vec4 outerCutoff;

	vec4 shadowCaster;
	vec4 isDirty;
	vec4 offset;
};

layout(binding = 0, std430) buffer LightsSSBO 
{
	Light lights[];
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// misc
uniform bool hasAOTexture;
uniform vec2 textureScaling;
vec2 scaledTexCoords;

uniform vec3 camPos;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
vec3 CalcPointLight(Light light, vec3 N, vec3 V, vec3 F0);
//vec3 CalcDirLight(Light light, vec3 N, vec3 V, vec3 F0);
//vec3 CalcSpotLight(Light light, vec3 N, vec3 V, vec3 F0);
float PointShadowCalculation(vec3 fragPos, Light light, int shadowCasterIndex);
float DirShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
vec3 getNormalFromMap();

vec3 albedo;
vec3 emission;
float roughness; 
float metallic;
float ao;
vec3 F0;

vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }

bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

void main(){
	if(!isInsideCube(FragPos, 0)) return;

	scaledTexCoords = TexCoords * textureScaling;

	vec4 albedoSample = texture(material.texture_albedo1, scaledTexCoords).rgba;
	vec3 RMA = texture(material.texture_rma1, scaledTexCoords).gbr;
	emission = texture(material.texture_emission1, scaledTexCoords).rgb;

	roughness = RMA.r;
	metallic = RMA.g;
	
	if(hasAOTexture)
	{
		//ao = RMA.b;
	}
	else
	{
		//ao = 1.0;
	}

	ao = 1.0;

	if(albedoSample.a < 0.5)
		discard;

	albedo = albedoSample.rgb;

	vec3 N = getNormalFromMap();
	vec3 V = normalize(camPos - FragPos); // viewDir
	vec3 R = reflect(-V, N);   

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0); 

	// Calculate diffuse lighting fragment contribution.
	// ------ Point lights ---------
	for(int i = 0; i < lights.length(); i++) {
		
		if (lights[i].type == vec4(1.0))
		{
			Lo += CalcPointLight(lights[i], N, V, F0);
		}
	}

	// Output lighting to 3D texture.
	vec3 voxel = scaleAndBias(FragPos);
	ivec3 dim = imageSize(texture3D);
	//float alpha = pow(1 - material.transparency, 4); // For soft shadows to work better with transparent materials.
	vec4 res = vec4(Lo, 1);

	// notice that we are indexing into the 3d texture using a vector of integers. So dim * voxel is truncated into an integer, which means that 
	// ivec3(dim * voxel) points to an exact voxel location in the 3d texture.
    imageStore(texture3D, ivec3(dim * voxel), res);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom  * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 CalcPointLight(Light light, vec3 N, vec3 V, vec3 F0) 
{
	float shadow = 0;

	vec3 L = normalize(light.position.xyz - FragPos); // lightDir
	vec3 H = normalize(V + L); // halfway vector

	float distance = length(light.position.xyz - FragPos);
	float attenuation = smoothstep(light.radius.x, 0.0, length(light.position.xyz - FragPos));
	vec3 radiance = light.color.xyz * attenuation; // the scaling by the angle between the normal of the surface and the solid angle (which is just the direction vector
	// of the fragment to the light in this case) is in the final reflectance formula.

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	// Cook-Torrance BRDF --> gives us how much each individual light ray (direction vector from fragment to light in this case) contributes 
	// to the final refleted light of an opaque surface (the current fragment) given its material properties.
	//vec3 numerator = NDF * G * F;
	//float denominator = 4.0 * max(dot(N, V), 0.0) *  max(dot(N, L), 0.0) + 0.0001;
	
	//vec3 specular = numerator / denominator;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;

	// metallic surfaces don't refract light, so they have no diffuse reflections
	kD *= 1.0 - metallic;

	float NdotL = max(dot(N, L), 0.0); // scale the light's contribution by its angle to the surface's normal.
		
	return (kD * albedo / PI) * radiance * NdotL;
}

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(material.texture_normal1, scaledTexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}