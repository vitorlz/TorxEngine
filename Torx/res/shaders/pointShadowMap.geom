#version 460 core 
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

// we send the fragPos to the fragment shader because we need it to calculate a depth value.
out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
	// remember that the geometry shader receives a primitive (in this case a triangle) and stores info about the vertices of this primitive
	// in an array called gl_in[]
	
	// the code below loops through all the faces of the cube map and uses them to index the shadowMatrices uniform array that contain the 
	// light space matrix for each of the cubemap faces. So, for each face, the loop sets the gl_Layer variable which specifies to which cubemap 
	// face we are going to render the primitive to (ex: 0 is right face, etc...), and then it iterates through the vertices of the triangle primitive
	// received by this geometry shader (the vertices' info is stored in the array gl_in[]) and multiplies the vertex positions by the light space
	// matrix corresponding to the respective face, essentially transforming the vertex position to light space. It transforms the vertices of 
	// the received prmitive to all 6 faces of the cubemap. We output 6 triangles (18 vertices). 

	for(int face = 0; face < 6; ++face)
	{
		gl_Layer = face; // built-in variable that specified to which face we render.
		for(int i = 0; i < 3; ++i) // for each triangle vertex
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face] * FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}

