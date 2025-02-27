#version 460 core
layout (location = 0) in vec2 vertex; // <vec2 pos, vec2 tex>

out vec2 TexCoords;
out flat int index;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 worldPos;

void main()
{
    gl_Position = projection * view  * worldPos * vec4(vertex.xy, 0.0, 1.0);
    index = gl_InstanceID;
    TexCoords = vertex.xy;
    TexCoords.y = 1.0 - TexCoords.y;
}  