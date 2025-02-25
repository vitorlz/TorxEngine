#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);

    // We don't want to render the skybox before everything because then we would be rendering fragments that would soon
    // be overwritten by other objects, and that is not very efficient. We want to only render the parts of the skybox that are going
    // to appear on the screen. In order to do that we have to set the depth of every vertex of the skybox to the maximum depth
    // value: 1. After the vertex positions leave the vertex shader, they get interpolated across the surface of primitives
    // and perspective division happens. The z component of the resulting division is equal to that fragment's depth value (kind of,
    // because it will get transformed to a range of 0,1 after). If we set the z component equal to the homogeneous coordinate w,
    // the depth value of the fragment will always be 1 (the highest value), so other objects will always overwrite skybox fragments.
    // We only set z = w after we apply view and projection transformations though, because then the skybox's faces will be rendered at the right
    // place. 

    gl_Position = pos.xyww;
}