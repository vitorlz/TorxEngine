#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

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
out vec3 ViewPosition;
//out vec4 ViewFragPos;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

bool hasBones = false;

void main()
{
    
    vec3 totalNormal = vec3(0.0);

    vec4 totalPosition = vec4(0.0);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[0] == boneIds[1] && boneIds[1] == boneIds[2] && boneIds[2] == boneIds[3])
        {
            totalPosition = vec4(aPos, 1.0);
            totalNormal = aNormal;
            break;
        }

        hasBones = true;
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(aPos, 1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = transpose(inverse(mat3(model * finalBonesMatrices[boneIds[i]]))) * aNormal;

        totalNormal += localNormal;
    }

    gl_Position = projection * view * model * totalPosition;
    TexCoords = aTexCoords;
    FragPos = vec3(model * totalPosition);
    ViewPosition =  vec3(view * model * totalPosition);
    Normal = hasBones ? totalNormal : normalMatrix * totalNormal; 
    FragPosLightSpaceDir = dirLightSpaceMatrix * model * totalPosition;
}