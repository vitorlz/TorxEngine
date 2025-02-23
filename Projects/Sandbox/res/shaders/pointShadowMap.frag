#version 460 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

// For directional shadows we used an empty fragment shader and let OpenGL figure out the depth values of the depth map. 
// This time we're going to calculate our own (linear) depth as the linear distance between each closest fragment position and the 
// light source's position. Calculating our own depth values makes the later shadow calculations a bit more intuitive. Remember that
// for directional shadows we used ortographic projection, which means that the depth values stored in the buffer were linear. Now we are using
// perspective projection for omnidirectional lighting, so the depth values are not stored linearly by default and we need to calculate
// linear depth values ourselves.

void main()
{
	// get distance bewteen fragment and light source
	float lightDistance = length(FragPos.xyz - lightPos);

	// map to [0,1] range by dividing by far_plane. Remember that fragments are already clipped, which means that the fragment shader will
	// only process fragments that are within the light's projection fustrum. This means that no fragment will have a distance greater than
	// the far plane relative to the light at this point. 
	lightDistance = lightDistance / far_plane;

	// write this as modified depth
	gl_FragDepth = lightDistance;

}