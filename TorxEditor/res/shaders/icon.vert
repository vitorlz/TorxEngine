#version 460 core
layout (location = 0) in vec2 vertex; // <vec2 pos, vec2 tex>

out vec2 TexCoords;
out flat int index;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 worldPos;
uniform float scale;
uniform vec3 iconOffset;

void main()
{
    vec3 pos = worldPos + transpose(mat3(view)) * ((vec3(vertex.xy, 0.0) + vec3(-0.25, -0.5, 0.0)) * scale);
    gl_Position = projection * view  * (vec4(pos + iconOffset, 1.0));

    index = gl_InstanceID;
    TexCoords = vertex.xy;
    TexCoords.y = 1.0 - TexCoords.y;
}  