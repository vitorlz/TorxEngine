#version 460 core
layout (location = 0) out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

// material parameters
struct Material 
{
	sampler2D texture_albedo1;

	sampler2D texture_normal1;

	sampler2D texture_roughness1;

	sampler2D texture_metallic1;

	sampler2D texture_diffuse1;
	
	sampler2D texture_ao1;
};

uniform Material material;
//in mat3 TBN;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

//lights

struct Light
{
	vec4 type;
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
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
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
vec3 CalcPointLight(Light light, vec3 N, vec3 V, vec3 F0);


vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(material.texture_normal1, TexCoords).xyz * 2.0 - 1.0;

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

vec3 albedo;
float roughness; 
float metallic;
float ao;
vec3 F0;

void main()
{
	vec4 albedoSample = texture(material.texture_diffuse1, TexCoords).rgba;
	// assimp apparently puts roughness in the green value of the roughness texture and metallic in the blue value of the 
	// roughness texture.
    metallic = texture(material.texture_roughness1, TexCoords).b;
    roughness = texture(material.texture_roughness1, TexCoords).g;
    ao = texture(material.texture_ao1, TexCoords).r;
	ao = 1;

	if(albedoSample.a < 0.5)
		discard;

	vec3 albedo = albedoSample.rgb;

	vec3 N = getNormalFromMap();
	vec3 V = normalize(camPos - FragPos); // viewDir
	vec3 R = reflect(-V, N);   

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0); // reflectance (total sum of radiance that comes from light sources and get reflected by a point P (the fragment in this case)

	// ------ Point lights ---------
	for(int i = 0; i < lights.length(); i++) {
		if (lights[i].type == vec4(1.0))
		{
			Lo += CalcPointLight(lights[i], N, V, F0);
		}
	}

	// We now use the value sampled from our irradiance map as the indirect diffuse lighting, which we put in the ambient component of the light.
	// indirect lighting has both a diffuse and specular part, so we need to weigh both parts according to the indirect reflectance (specular) ratio 
	// and indirect refractive (diffuse) ratio. We use the fresnelSchlick formula to find the reflectance ratio and use that to find the refractive ratio.
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
	// retrieve the irradiance influencing the fragment
	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;
	// We treat both diffuse and specular indirect lighting as ambient lighting as it comes from outside sources and not from direct light sources.
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	
	vec3 ambient = (kD * diffuse + specular) * ao;
	
	vec3 color = ambient + Lo;

	FragColor = vec4(color, 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
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
		vec3 L = normalize(light.position.xyz - FragPos); // lightDir
		vec3 H = normalize(V + L); // halfway vector

		float distance = length(light.position.xyz - FragPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = light.diffuse.xyz * attenuation; // the scaling by the angle between the normal of the surface and the solid angle (which is just the direction vector
		// of the fragment to the light in this case) is in the final reflectance formula.

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		// Cook-Torrance BRDF --> gives us how much each individual light ray (direction vector from fragment to light in this case) contributes 
		// to the final refleted light of an opaque surface (the current fragment) given its material properties.
		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) *  max(dot(N, L), 0.0) + 0.0001;
		// remember that the Cook-Torrance BRDF has both a diffuse and specular part, this is the specular part (the part of the light ray that gets reflected)
		// by the surface)
		vec3 specular = numerator / denominator;
		// the diffuse (part of the light ray that is refracted / absorbed by the surface) is just 1 - Ks. Ks is the ratio of light that is reflected, which is the same as
		// F that we calculated above and is part of the BRDF.

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;

		// metallic surfaces don't refract light, so they have no diffuse reflections
		kD *= 1.0 - metallic;
		// now we can calculate each light's contribution to the reflectance equation. Notice that here we are not taking the integral because we know the wi (the solid angle / 
		// direction vector) of the fragment to all light sources to the fragment. We also know that each source has only a single direction vector (light ray) that influences the 
		// fragment, so we can just loop through the light sources and calculate their irradiance. When we take environment lighting into account we will have to take the integral
		// because light can come from any direction.
		
		// The below is the outgoing reflectance value for a single light. We add it to the final reflectance result (that will contain the sum of the reflectance value of all the 
		// lights that would be calculated by taking the integral over the hemisphere around P (the fragment).

		float NdotL = max(dot(N, L), 0.0); // scale the light's contribution by its angle to the surface's normal.
		
		
		return (kD * albedo / PI + specular) * radiance * NdotL;

		//return vec3(roughness);

		// Kd * albedo / PI  is the diffuse part of the BRDF.
		// The resulting Lo value, or the outgoing radiance, is effectively the result of the reflectance equation's integral over the hemisphere around P. We don't really have to 
		// try and solve the integral for all possible incoming light directions as we know exactly the 4 incoming light directions that can influence the fragment. Because of this, 
		// we can  directly loop over these incoming light directions e.g. the number of lights in the scene.
}


