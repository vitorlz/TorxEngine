#version 460 core
out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
	// In this fragment shader we convolute the cubemap we created from the hdr texture in order to create an irradiance map from which we can
	// sample to retrieve indirect diffuse lighting (irradiance / sum of radiance) for each fragment. We do that by sampling from the cubemap
	// in multiple directions inside a hemisphere at a given orientation, we then average the values sampled to get the irradiance. We store that irradiance
	// in another cubemap (an irradiance map) and when we want to get to irradiance (indirect diffuse lighting) for a given fragment we simply sample
	// from this irradiance map at the given fragment's surface normal direction. This is basically an approximation of the reflectance equation for each
	// texel of the cubemap.

	// We are going to draw this texture onto a cube and then render the cube 6 times, each time looking at a different face in order to populate the cubemap
	// (irradiance map) by using its faces as the framebuffer's color textures each time we render the cube.

	vec3 normal = normalize(localPos);	

	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal, right));
	// we traverse the hemisphere using this sampleDelta value. Increasing it will increase accuracy.
	float sampleDelta = 0.005;
	float nrSamples = 0.0;
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to carthesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	FragColor = vec4(irradiance, 1.0);
}