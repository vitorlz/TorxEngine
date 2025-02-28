#version 460 core
layout (location = 0) out vec4 FragColor;
in vec3 localPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(equirectangularMap, uv).rgb; // we use a spherical to cartesian coordinate conversion so that we can sample from the
    // equirectangular map as if it was a cubemap itself. This way we can properly sample the correct color for each face of the cube. and 
    // it will look like the equirectangular map is neatly folded onto the cube. We use spherical to carthesian because we are using a direction vector
    // that is a unit vector (localPos in this case) to sample the correct color. If you imagine an unit direction vector rotating around a point (the
    // origin of the cube) it will make a sphere. So when we sample a cubemap for the skybox, for example, if we render in all direction vectors 
    // around the origin with which we sample from the cubemap, it will form a sphere.

    FragColor = vec4(color, 1.0);
}