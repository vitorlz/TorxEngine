#version 460 core

#define TSQRT2 2.828427
#define SQRT2 1.414213
#define ISQRT2 0.707106
#define DIFFUSE_INDIRECT_FACTOR 0.1f
#define MIPMAP_HARDCAP 5.4

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 ViewPosition;
layout (location = 3) out vec4 ViewNormals;
layout (location = 4) out vec4 Roughness;
layout (location = 5) out vec4 DiffuseColor;

in vec2 TexCoords;	
in vec3 FragPos;
in vec3 Normal;

// material parameters
struct Material 
{
	sampler2D texture_albedo1;

	sampler2D texture_normal1;

	sampler2D texture_rma1;

	sampler2D texture_emission1;
};

uniform Material material;

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
in vec4 FragPosLightSpaceDir;
uniform samplerCubeArray pointShadowMap;
int pointShadowCasterIndex = 0;

// misc
uniform bool hasAOTexture;
uniform vec2 textureScaling;
vec2 scaledTexCoords;

// SSR
uniform sampler2D gViewPos;
uniform sampler2D gViewNormals;
in mat4 ProjectionMatrix;
in vec4 ViewFragPos;
uniform mat3 viewNormalMatrix;
float rayStep = 0.1;
float minRayStep = 0.1;
int maxSteps = 30;
float searchDist = 5;
int numBinarySearchSteps = 5;
float reflectionSpecularFalloffExponent = 3.0;

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
vec3 scaleAndBias(const vec3 p);
vec3 indirectDiffuseLight(vec3 normal);
bool isInsideCube(const vec3 p, float e);
vec3 orthogonal(vec3 u);
vec3 traceDiffuseVoxelCone(const vec3 from, vec3 direction);
vec3 traceSpecularVoxelCone(vec3 from, vec3 direction, vec3 normal);
vec3 indirectSpecularLight(vec3 viewDirection, vec3 normal);
vec4 ssrRayCast(vec3 dir, vec3 hirCoord, float dDepth);

vec3 albedo;
vec3 emission;
float roughness; 
float metallic;
float ao;
vec3 F0; 

// vxgi
uniform sampler3D voxelTexture;
uniform float voxelSize;
uniform bool vxgi;
uniform bool showDiffuseAccumulation;
uniform bool showTotalIndirectDiffuseLight;
uniform float diffuseConeSpread;
uniform float voxelizationAreaSize;
uniform float specularBias;
uniform float specularStepSizeMultiplier;
uniform float specularConeOriginOffset;
uniform bool showTotalIndirectSpecularLight;
uniform float specularConeMaxDistance;

// SSR 

uniform sampler2D ssrTexture;

float MAX_DISTANCE;

void main()
{	
	scaledTexCoords = TexCoords * textureScaling;

	MAX_DISTANCE = distance(vec3(abs(FragPos)), vec3(-specularConeMaxDistance));

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

	vec3 color = Lo;

	vec3 indirectDiffuseContribution;
	vec3 indirectSpecularContribution;

	vec2 texSize = textureSize(ssrTexture, 0);
	float mipLevel = pow(roughness, 2.0) * 3 + 0.5;
	vec3 ssr = textureLod(ssrTexture, gl_FragCoord.xy / texSize, mipLevel).rgb;

	if(vxgi)
	{
		indirectDiffuseContribution = (kD * indirectDiffuseLight(N));
		indirectSpecularContribution = (kS * ssr);
	}
	if(vxgi && !(showTotalIndirectDiffuseLight || showDiffuseAccumulation || showTotalIndirectSpecularLight))
	{
		color += indirectDiffuseContribution + indirectSpecularContribution;	
	}
	else if(showTotalIndirectDiffuseLight || showDiffuseAccumulation)
	{
		color = indirectDiffuseContribution;
	}
	else if(showTotalIndirectSpecularLight)
	{
		color = ssr;
	}
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


	Roughness = vec4(vec3(roughness), 1.0);
	ViewPosition = ViewFragPos;
	ViewNormals = vec4(viewNormalMatrix * N, 1.0);
	DiffuseColor = vec4(Lo + indirectDiffuseContribution, 1.0);
}


vec4 ssrRayCast(vec3 dir, vec3 hitCoord, float dDepth)
{
	dir *= rayStep;
	
	float depth = 0.0;
	int steps = 0;
	vec4 projectedCoord = vec4(0.0);

	for(int i = 0; i < maxSteps; ++i)
	{
		hitCoord += dir;

		projectedCoord = ProjectionMatrix * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

		
	}
	return vec4(1.0);
}

// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
vec3 scaleAndBias(const vec3 p) { return 0.5f * p + vec3(0.5f); }

// Returns true if the point p is inside the unity cube. 
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

// Returns a soft shadow blend by using shadow cone tracing.
// Uses 2 samples per step, so it's pretty expensive.

vec3 orthogonal(vec3 u){
	u = normalize(u);
	vec3 v = vec3(0.99146, 0.11664, 0.05832); // Pick any normalized vector.
	return abs(dot(u, v)) > 0.99999f ? cross(u, vec3(0, 1, 0)) : cross(u, v);
}

// Traces a diffuse voxel cone.
vec3 traceDiffuseVoxelCone(const vec3 from, vec3 direction){
	direction = normalize(direction);

	float CONE_SPREAD = diffuseConeSpread;

	vec4 acc = vec4(0.0f);

	// Controls bleeding from close surfaces.
	// Low values look rather bad if using shadow cone tracing.
	// Might be a better choice to use shadow maps and lower this value.
	float dist = 0.0;

	// Trace.
	while(dist < SQRT2 && acc.a < 1){
		vec3 c = (from + dist * direction) / voxelizationAreaSize;
		if(!isInsideCube(c, 0)) break;
		c = scaleAndBias(c);		
		float l = 1 + CONE_SPREAD * dist / voxelSize;
		float level = log2(l);
		float ll = (level + 1) * (level + 1);
		vec4 voxel = textureLod(voxelTexture, c, min(MIPMAP_HARDCAP, level));
		acc += 0.075 * ll * voxel * pow(1 - voxel.a, 2);
		dist += ll * voxelSize * 2;
	}
	return pow(acc.rgb * 2.0, vec3(1.5));
}

vec3 indirectDiffuseLight(vec3 normal){
	const float ANGLE_MIX = 0.666; // Angle mix (1.0f => orthogonal direction, 0.0f => direction of normal).

	const float w[3] = {1.0, 1.0, 1.0}; // Cone weights.

	// Find a base for the side cones with the normal as one of its base vectors.
	const vec3 ortho = normalize(orthogonal(normal));
	const vec3 ortho2 = normalize(cross(ortho, normal));

	// Find base vectors for the corner cones too.
	const vec3 corner = 0.5f * (ortho + ortho2);
	const vec3 corner2 = 0.5f * (ortho - ortho2);

	// Find start position of trace (start with a bit of offset).
	const vec3 N_OFFSET = normal * (1 + 4 * ISQRT2) * voxelSize;
	const vec3 C_ORIGIN = FragPos + N_OFFSET;

	// Accumulate indirect diffuse light.
	vec3 acc = vec3(0);

	// We offset forward in normal direction, and backward in cone direction.
	// Backward in cone direction improves GI, and forward direction removes
	// artifacts.
	const float CONE_OFFSET = -0.01;

	// Trace front cone
	acc += w[0] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * normal, normal);
		
	// Trace 4 side cones.
	const vec3 s1 = mix(normal, ortho, ANGLE_MIX);
	const vec3 s2 = mix(normal, -ortho, ANGLE_MIX);
	const vec3 s3 = mix(normal, ortho2, ANGLE_MIX);
	const vec3 s4 = mix(normal, -ortho2, ANGLE_MIX);

	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho, s1);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho, s2);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho2, s3);
	acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho2, s4);

	// Trace 4 corner cones.
	const vec3 c1 = mix(normal, corner, ANGLE_MIX);
	const vec3 c2 = mix(normal, -corner, ANGLE_MIX);
	const vec3 c3 = mix(normal, corner2, ANGLE_MIX);
	const vec3 c4 = mix(normal, -corner2, ANGLE_MIX);

	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner, c1);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner, c2);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner2, c3);
	acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner2, c4);

	// Return result.

	if(showDiffuseAccumulation)
	{
		return  acc;
	}
	else 
	{
		return acc * (albedo + vec3(0.001f));
	}
}

vec3 traceSpecularVoxelCone(vec3 from, vec3 direction, vec3 normal) {
    direction = normalize(direction);

    const float OFFSET = specularConeOriginOffset * voxelSize;
    const float STEP = voxelSize * specularStepSizeMultiplier;

    // Offset the starting point to prevent self-reflection artifacts
    from += OFFSET * normal;

	float angleFactor = log2(clamp(1 - dot(direction, normal), 0.0, 1.0) * 10);

    vec4 acc = vec4(0.0);
    float dist = OFFSET;

    // Trace the voxel cone
    while (dist < MAX_DISTANCE && acc.a < 1.0) { 
        vec3 c = (from + dist * direction) / voxelizationAreaSize;
        if (!isInsideCube(c, 0)) break;
        c = scaleAndBias(c);
		// introduce an angle factor to hide artifact of smooth surfaces reflections when viewed at an angle
        float level = max(roughness * log2(1 + dist / voxelSize) / specularBias, 0.0);
		vec4 voxel = textureLod(voxelTexture, c, min(MIPMAP_HARDCAP, level));
        float contributionFactor = 1.0 - acc.a;
        acc.rgb += 0.25 * (1.0 + roughness) * voxel.rgb * voxel.a * contributionFactor;
        acc.a += 0.25 * voxel.a * contributionFactor;
        dist += STEP * (1.0 + 0.125 * level);
    }

    return pow(roughness + 1.0, 0.8) * acc.rgb;
}

vec3 indirectSpecularLight(vec3 viewDirection, vec3 normal) {
    const vec3 reflection = normalize(reflect(-viewDirection, normal));
    float grazingAngleFactor = max(dot(normal, reflection), 0.05); // Avoid near-zero artifacts

    // Add grazing angle falloff to reduce artifacts
    return (1.0 - roughness) * specularBias * traceSpecularVoxelCone(FragPos, reflection, normal) * grazingAngleFactor;
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
		closestDepth *= light.radius.x;   // undo mapping [0;1]
		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples) * 1.1;  
		
	float unitDepth = currentDepth / light.radius.x;

	return mix(shadow, 0, unitDepth / 10);
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
