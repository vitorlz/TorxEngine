#version 460 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

// interpolated fragment's directional light's light space position
in vec4 FragPosLightSpaceDir;

// material parameters
struct Material 
{
	sampler2D texture_albedo1;

	sampler2D texture_normal1;

	sampler2D texture_rma1;

	sampler2D texture_emission1;
};

uniform Material material;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// debug
uniform bool showNormals;
uniform bool worldPosDebug;
uniform bool bloom;
uniform bool albedoDebug;
uniform bool roughnessDebug;
uniform bool metallicDebug;
uniform bool aoDebug;
uniform bool emissionDebug;

// shadows
uniform sampler2D dirShadowMap;
uniform samplerCubeArray pointShadowMap;
uniform float point_far_plane[10];
int pointShadowCasterIndex = 0;

// misc
uniform bool hasAOTexture;
uniform vec2 textureScaling;
vec2 scaledTexCoords;

//lights
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

uniform vec3 camPos;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
vec3 CalcPointLight(Light light, vec3 N, vec3 V, vec3 F0);
vec3 CalcDirLight(Light light, vec3 N, vec3 V, vec3 F0);
vec3 CalcSpotLight(Light light, vec3 N, vec3 V, vec3 F0);
float PointShadowCalculation(vec3 fragPos, Light light, int shadowCasterIndex);
float DirShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
vec3 getNormalFromMap();

vec3 albedo;
vec3 emission;
float roughness; 
float metallic;
float ao;
vec3 F0;

void main()
{

	scaledTexCoords = TexCoords * textureScaling;

	vec4 albedoSample = texture(material.texture_albedo1, scaledTexCoords).rgba;
	vec3 RMA = texture(material.texture_rma1, scaledTexCoords).gbr;
	emission = texture(material.texture_emission1, scaledTexCoords).rgb;

	
	roughness = RMA.r;
	metallic = RMA.g;
	
	if(hasAOTexture)
	{
		ao = RMA.b;
	}
	else
	{
		ao = 1.0;
	}

	if(albedoSample.a < 0.5)
		discard;

	albedo = albedoSample.rgb;

	vec3 N = getNormalFromMap();
	vec3 V = normalize(camPos - FragPos); // viewDir
	vec3 R = reflect(-V, N);   

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0); // reflectance (total sum of radiance that comes from light sources and get reflected by a point P (the fragment in this case)


	// ------ Directional Light ---------
	for(int i = 0; i < lights.length(); i++) {
		if (lights[i].type == vec4(0.0))
		{
			Lo += CalcDirLight(lights[i], N, V, F0);
		}
	}

	// ------ Point lights ---------
	for(int i = 0; i < lights.length(); i++) {
		if (lights[i].type == vec4(1.0))
		{
			Lo += CalcPointLight(lights[i], N, V, F0);
		}
	}

	// ------ Spot lights ---------
	for(int i = 0; i < lights.length(); i++) {
		if (lights[i].type == vec4(2.0))
		{
			Lo += CalcSpotLight(lights[i], N, V, F0);
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
	
	//vec3 ambient = (kD * diffuse + specular) * ao;
	vec3 ambient = ((kD * diffuse + specular) * ao) / 10;
	
	vec3 color = Lo;

	if(showNormals) 
	{
		FragColor = vec4(N, 1.0);
	}
	else if (worldPosDebug) 
	{
		FragColor = vec4(FragPos, 1.0);
	}
	else if (albedoDebug)
	{
		FragColor = vec4(albedo, 1.0);
	}
	else if (roughnessDebug)
	{
		FragColor = vec4(vec3(roughness), 1.0);
	}
	else if (metallicDebug)
	{
		FragColor = vec4(vec3(metallic), 1.0);
	}
	else if (aoDebug)
	{
		FragColor = vec4(vec3(ao), 1.0);
	}
	else if (emissionDebug)
	{
		FragColor = vec4(emission, 1.0);
	}
	else 
	{
		FragColor = vec4(color + emission * 2, 1.0);	

		if (bloom)
		{
			float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
			if(brightness > 1.0)
				BrightColor = vec4(FragColor.rgb, 1.0);
			else
				BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
		}
	}
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
	float shadow = 0;

	if (light.shadowCaster.x == 1) 
	{
		shadow = PointShadowCalculation(FragPos, light, pointShadowCasterIndex);
		pointShadowCasterIndex++;
	}

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
		
	return (kD * albedo / PI + specular) * radiance * NdotL * clamp((1.0 - shadow), 0.0, 1.0);

	//return vec3(roughness);

	// Kd * albedo / PI  is the diffuse part of the BRDF.
	// The resulting Lo value, or the outgoing radiance, is effectively the result of the reflectance equation's integral over the hemisphere around P. We don't really have to 
	// try and solve the integral for all possible incoming light directions as we know exactly the 4 incoming light directions that can influence the fragment. Because of this, 
	// we can  directly loop over these incoming light directions e.g. the number of lights in the scene.
}


vec3 CalcDirLight(Light light, vec3 N, vec3 V, vec3 F0) 
{

	// same thing as point light calculation, but the light direction L does not depend on the light position. It is a constant direciton.
	// There is also no attenuation

	vec3 L = normalize(light.position.xyz); // lightDir

	float shadow = 0;

	if (light.shadowCaster.x == 1) 
	{
		shadow = DirShadowCalculation(FragPosLightSpaceDir, N, L);
	}

	vec3 H = normalize(V + L); // halfway vector

	vec3 radiance = light.color.xyz;

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) *  max(dot(N, L), 0.0) + 0.0001;
		
	vec3 specular = numerator / denominator;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - metallic;

	float NdotL = max(dot(N, L), 0.0); // scale the light's contribution by its angle to the surface's normal.
		
	return (kD * albedo / PI + specular) * radiance * NdotL * clamp(1.0 - shadow, 0.0, 1.0);
}

vec3 CalcSpotLight(Light light, vec3 N, vec3 V, vec3 F0) 
{
	vec3 L = normalize(light.position.xyz - FragPos); // lightDir
	vec3 H = normalize(V + L); // halfway vector

	float distance = length(light.position.xyz - FragPos);
	float attenuation = smoothstep(light.radius.x, 0.0, length(light.position.xyz - FragPos));
	float theta = dot(normalize(-light.direction.xyz), L);
	float episilon = light.innerCutoff.x - light.outerCutoff.x;
	float intensity = clamp((theta - light.outerCutoff.x) / episilon, 0.0, 1.0); 
	vec3 radiance = light.color.xyz * attenuation * intensity; 

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) *  max(dot(N, L), 0.0) + 0.0001;
	
	vec3 specular = numerator / denominator;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;

	// metallic surfaces don't refract light, so they have no diffuse reflections
	kD *= 1.0 - metallic;

	float NdotL = max(dot(N, L), 0.0); // scale the light's contribution by its angle to the surface's normal.
		
	return (kD * albedo / PI + specular) * radiance * NdotL;

}

float PointShadowCalculation(vec3 fragPos, Light light, int shadowCasterIndex) 
{
	// get a vector from the light source to the current fragment so that we can (1) get its length to use as the depth value of the current fragment (remember that we used the length
	// of a vector from the light to a fragment as the depth value when generating the shadow map to make the depth values linear. We use perspective projection when rendering the scene
	// to create point shadow map, so the depth values stored would not be linear, and that is why we had to do it ourselves.
	// (2) we also use this vector to sample a depth value from the shadow cubemap.

	vec3 fragToLight = fragPos - light.position.xyz;

	float currentDepth = length(fragToLight);

	vec3 sampleOffsetDirections[56] = vec3[]
	(
		vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
		vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
		vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
		vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
		vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1),
		vec3( 0.5,  0.5,  0.5), vec3(-0.5, -0.5,  0.5), vec3(0.5, -0.5, 0.5), vec3(-0.5, 0.5, 0.5),
		vec3( 0.5,  0.5, -0.5), vec3(-0.5, -0.5, -0.5), vec3(0.5, -0.5, -0.5), vec3(-0.5, 0.5, -0.5),
		vec3( 0.5,  1,  0), vec3(-0.5,  1,  0), vec3(0.5, -1, 0), vec3(-0.5, -1, 0),
		vec3( 1,  0.5,  0), vec3(-1,  0.5,  0), vec3(1, -0.5, 0), vec3(-1, -0.5, 0),
		vec3( 0,  0.5,  1), vec3(0, -0.5, 1), vec3(0, 0.5, -1), vec3(0, -0.5, -1),
		vec3( 0.5,  0,  1), vec3(-0.5,  0,  1), vec3(0.5, 0, -1), vec3(-0.5, 0, -1),
		vec3( 0.25,  0.25,  0.25), vec3(-0.25, -0.25,  0.25), vec3(0.25, -0.25, 0.25), vec3(-0.25, 0.25, 0.25),
		vec3( 0.25,  0.25, -0.25), vec3(-0.25, -0.25, -0.25), vec3(0.25, -0.25, -0.25), vec3(-0.25, 0.25, -0.25),
		vec3( 1,  0,  0), vec3(-1,  0,  0), vec3(0, 1, 0), vec3(0, -1, 0)
	);

	// this is PCF but in 3D. We sample a "disk" around a given texel, test to see if it is in shadow or not, add the result to the shadow variable,
	// and then take the average. This gets us smoother shadows.

	float shadow = 0.0;
	float bias   = 0.0;	
	int samples  = 56;
	// this diskradis is what is causing light bleed.
	float diskRadius = 0.01;
	for(int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(pointShadowMap, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius, shadowCasterIndex)).r;
		closestDepth *= point_far_plane[shadowCasterIndex];   // undo mapping [0;1]
		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples) * 1.1;  

	float unitDepth = currentDepth / point_far_plane[shadowCasterIndex];

	return mix(shadow, 0, unitDepth / 3);
}

float DirShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // this returns the fragment's light space position in the range [-1, 1]

	projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(dirShadowMap, projCoords.xy).r;

	float currentDepth = projCoords.z;

	float bias = max(0.04 * (1.0 - dot(normal, lightDir)), 0.00015);

	float shadow = 0.0;
	vec2 texelSize = 0.2 / textureSize(dirShadowMap, 0);
	for(float x = -5.5; x <= 5.5; ++x)
	{
		for(float y = -5.5; y <= 5.5; ++y)
		{
			float pcfDepth = texture(dirShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 90;

	if(projCoords.z > 1.0)
        shadow = 0.0;

	return shadow;
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
