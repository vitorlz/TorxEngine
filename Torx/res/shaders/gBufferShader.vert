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
out vec3 ViewPosition;
//out vec4 ViewFragPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    ViewPosition =  vec3(view * model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;
    //ViewFragPos = view * model * vec4(aPos, 1.0);
    FragPosLightSpaceDir = dirLightSpaceMatrix * model * vec4(aPos, 1.0);

}