#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 24) out;

in vec3 gridCoord[];

uniform vec3 voxelSize; // The size of each voxel.
uniform vec4 color;     // The color of the voxel.

out vec4 fragColor;

void emitCubeFace(vec3 base, vec3 offset1, vec3 offset2, vec4 color) {
    // Emit 4 vertices to form a face of the cube.
    gl_Position = gl_in[0].gl_Position + vec4(base, 0.0) + vec4(offset1, 0.0);
    fragColor = color;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(base, 0.0) + vec4(offset2, 0.0);
    fragColor = color;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(base + offset1, 0.0);
    fragColor = color;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(base + offset2, 0.0);
    fragColor = color;
    EmitVertex();

    EndPrimitive();
}

void main() {
    // Calculate the base of the cube from gridCoord.
    vec3 base = gridCoord[0];
    
    // Define the 6 faces of the cube.
    vec3 right = vec3(voxelSize.x, 0.0, 0.0);
    vec3 up = vec3(0.0, voxelSize.y, 0.0);
    vec3 forward = vec3(0.0, 0.0, voxelSize.z);

    // Define offsets for each face.
    // Right face (X+)
    emitCubeFace(base, right, up, color);

    // Left face (X-)
    emitCubeFace(base, -right, -up, color);

    // Top face (Y+)
    emitCubeFace(base, right, forward, color);

    // Bottom face (Y-)
    emitCubeFace(base, -right, -forward, color);

    // Front face (Z+)
    emitCubeFace(base, up, forward, color);

    // Back face (Z-)
    emitCubeFace(base, -up, -forward, color);
}
