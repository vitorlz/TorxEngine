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
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 quadratic;
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

vec4 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
	
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 norm;

	//norm = normalize(Normal);

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

	// ------ Point lights ---------
	for(int i = 0; i < lights.length(); i++) {
		result += CalcPointLight(lights[i], norm, FragPos, viewDir);
	}

	float gamma = 2.2;

	vec3 color = pow(result.rgb, vec3(1/gamma));

   
    FragColor = vec4(color, 1.0);

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
	float attenuation = 1.0 / ( 1.0 + light.quadratic.x * (distance * distance)); //(light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec4 ambient = light.ambient * materialDiffuse;
	vec4 diffuse = light.diffuse * diff * materialDiffuse; //* (1.0 - shadow);
	vec4 specular = light.specular * spec * materialSpecular;// * clamp((1.0 - shadow * 2), 0.0, 1.0);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}
