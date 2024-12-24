#version 460 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec3 gRMA;
layout (location = 4) out vec4 gEmission;
layout (location = 5) out vec4 gDirLightSpacePosition;
layout (location = 6) out vec3 gViewPosition;
layout (location = 7) out vec3 gViewNormal;

in vec2 TexCoords;	
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpaceDir;
in vec3 ViewPosition;

uniform vec2 textureScaling;
uniform mat3 viewNormalMatrix;

struct Material 
{
	sampler2D texture_albedo1;

	sampler2D texture_normal1;

	sampler2D texture_rma1;

	sampler2D texture_emission1;
};

uniform Material material;

vec2 scaledTexCoords;

vec3 getNormalFromMap();

void main()
{
	scaledTexCoords = TexCoords * textureScaling;
	
	gPosition = FragPos;
	gNormal = getNormalFromMap();
	gAlbedo = texture(material.texture_albedo1, scaledTexCoords);
	gRMA = texture(material.texture_rma1, scaledTexCoords).gbr;
	gEmission = texture(material.texture_emission1, scaledTexCoords);
	gDirLightSpacePosition = FragPosLightSpaceDir;
	gViewPosition = ViewPosition;
	gViewNormal = viewNormalMatrix * gNormal;
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
