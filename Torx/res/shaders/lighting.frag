#version 460 core
layout (location = 0) out vec4 FragColor;

struct Material 
{
	sampler2D texture_diffuse1;

	sampler2D texture_specular1;

	sampler2D texture_normal1;

	sampler2D texture_emission1;

	sampler2D texture_height1;

	float shininess;
};

struct Light
{
	vec4 type;
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 quadratic;

	// for spotlight
	vec4 direction;
	vec4 innerCutoff;
	vec4 outerCutoff;
};

layout(binding = 0, std430) buffer LightsSSBO 
{
	Light lights[];
};

uniform Material material;
vec4 materialDiffuse;
vec4 materialSpecular;
vec4 materialEmission;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

uniform vec3 cameraPos;
uniform vec3 cameraFront;
uniform bool showNormals;
uniform bool worldPosDebug;

vec4 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec4 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
	
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 norm;

	norm = texture(material.texture_normal1, TexCoords).rgb;
	norm = norm * 2.0 - 1.0;
	norm = normalize(TBN * norm);

	materialDiffuse  = texture(material.texture_diffuse1, TexCoords);

	if (materialDiffuse.a < 0.5) {
		discard;
	}

	materialSpecular = texture(material.texture_diffuse1, TexCoords);				
	materialEmission = texture(material.texture_emission1, TexCoords);

	vec4 result;

	// ----- Directional Light ------
	for(int i = 0; i < lights.length(); i++) {
		if (lights[i].type == vec4(0.0))
			result += CalcDirLight(lights[i], norm, viewDir);
	}

	// ------ Point lights ---------
	for(int i = 0; i < lights.length(); i++) {
		if (lights[i].type == vec4(1.0))
			result += CalcPointLight(lights[i], norm, FragPos, viewDir);
	}

	// -------- Spotlight ------------
	for(int i = 0; i < lights.length(); i++) {
		if (lights[i].type == vec4(2.0)) 
			result += CalcSpotLight(lights[i], norm, FragPos, viewDir);
	}

	if(showNormals) 
		FragColor = vec4(norm, 1.0);
	else if (worldPosDebug) 
		FragColor = vec4(FragPos, 1.0);
	else 
		FragColor = result;	
}

vec4 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {

	// the function OmniShadowCalculation requires the light position to be in world space for its calculations. However, we are doing the lighting calculations
	// in view space, so we sent the light position in world space as an uniform and transformed it to view space here in the fragment shader.

	vec3 lightDir = normalize(light.position.xyz - fragPos);

	//float shadow = PointShadowCalculation(fragPos, light);

	// diffuse shading
	float diff = max(dot(lightDir, normal), 0.0);

	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	// attenuation
	float distance = length(light.position.xyz -  fragPos); // this is the distance between the fragment and the light 
	float attenuation = 1.0 / ( 1.0 + light.quadratic.x * (distance * distance)); 

	vec4 ambient = light.ambient * materialDiffuse;
	vec4 diffuse = light.diffuse * diff * materialDiffuse; //* (1.0 - shadow);
	vec4 specular = light.specular * spec * materialSpecular;// * clamp((1.0 - shadow * 2), 0.0, 1.0);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec4 CalcDirLight(Light light, vec3 normal, vec3 viewDir){
	
	// the direction of a directional light is constant. 
	vec3 lightDir = normalize(light.position.xyz);
	
	//float shadow = DirShadowCalculation(FragPosLightSpaceDir, normal, lightDir);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	
	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);
	
	vec4 ambient = light.ambient * materialDiffuse;
	vec4 diffuse = light.diffuse * diff * materialDiffuse; //* (1.0 - clamp(shadow, 0.0, 1.0));
	vec4 specular = light.specular * spec * materialSpecular;// * clamp((1.0 - shadow * 2), 0.0, 1.0);

	return (ambient + diffuse + specular);
}

vec4 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {

	//float shadow = spotShadowCalculation(FragPosLightSpaceSpot, FragPosWorld, normal, light.shadowCameraPos);
	// for more realistic spotlight you should probably calculate diffuse, specular, etc... accounting for the angle of the light and not only the light position. (I think)
	//	if you only move your mouse without changing the position of the camera, the specular highlights don't change now. I think they should change.(maybe im wrong)
	vec3 lightDir = normalize(light.position.xyz - fragPos);

	// diffuse shading
	float diff = max(dot(lightDir, normal), 0.0);

	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);

	// attenuation
	float distance = length(light.position.xyz -  fragPos); // this is the distance between the fragment and the light 
	float attenuation = 1.0 / ( 1.0 + light.quadratic.x * (distance * distance)); 

	// DON'T FORGET TO CLAMP THE INTENSITY BETWEEN 0 AND 1, OTHERWISE IT COULD BE NEGATIVE AND IT WOULD MESS UP THE CALCULATIONS.
	float theta = dot(normalize(-light.direction.xyz), lightDir);
	float episilon = light.innerCutoff.x - light.outerCutoff.x;
	float intensity = clamp((theta - light.outerCutoff.x) / episilon, 0.0, 1.0); 

	vec4 ambient = light.ambient * materialDiffuse;
	vec4 diffuse = light.diffuse * diff * materialDiffuse; //* (1.0 - shadow);
	vec4 specular = light.specular * spec * materialSpecular; //* clamp((1.0 - shadow * 2), 0.0, 1.0);

	ambient *= intensity * attenuation;
	diffuse *= intensity * attenuation;
	specular *= intensity * attenuation;

	return (ambient + diffuse + specular);
}