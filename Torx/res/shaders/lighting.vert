#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat3 normalMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    // calculate the normal matrix in the CPU to increase performance. Calculating inverse in the frag shader is expensive.
    mat3 normalMatrix = mat3(transpose(inverse(model)));

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;

}
