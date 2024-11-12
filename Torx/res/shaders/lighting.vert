#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

// directional light space matrix and vertex light space position
uniform mat4 dirLightSpaceMatrix;
out vec4 FragPosLightSpaceDir;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;
    FragPosLightSpaceDir = dirLightSpaceMatrix * model * vec4(aPos, 1.0);

    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 N = normalize(vec3(normalMatrix * aNormal));
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	// then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = normalize(cross(N, T));

    vec3 BHandness = normalize(aBiTangent);
            
    // Flip bitangent if mirrorred  (not 100% fixed)
    if (dot(B, BHandness) < 0.0) 
    {
        B = B * -1.0;
    } 
    

    TBN = mat3(T, B, N);    
}
