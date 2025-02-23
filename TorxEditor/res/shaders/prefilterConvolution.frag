#version 460 core
out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);

void main()
{
    // In this fragment shader we are creating a prefiltered environment map from which we are going to sample the specular indirect lighting. We take the environment map as input.
    // We create the sample vectors (which we use to sample from the environment cubemap) by using a low-discrepancy sequence and biasing them towards the specular lobe around the
    // microfacet halfway vector, which we do through GGX importance sampling based on a roughness value. So basically instead of sampling directions uniformly in a hemisphere like,
    // we did for diffuse irradiance, we are sampling in a more constrained area (specular lobe) whose size is defined by the roughness of the surface. We use the low-discrepancy 
    // sequence to get a random vector and then use importance sampling to get a sample vector in a zone around the microfacet's halfway vector. We do that multiple times so that we
    // get multiple samples and take their weighted average at the end. So we are basically solving an integral over the specular lobe discretely. Again, we are eventually going 
    // to store this prefiltered map in a cubemap. So we are going to do the same process of rendering the environment map using this convolution shader onto a unit cube and then
    // we are going to render the cube 6 times, each looking at a different cube face in order to populate the faces of the prefilteredCubemap which will be attached to the framebuffer's
    // color buffer. The difference here is that we have a roughness value, and we are going to generate mipmaps for this cubemap based on the roughness value. So when it comes time to
    // sample the specular indirect lighting from this map when rendering a 3d model, we can sample from the corresponding mip map based on the meshes' roughness value. In this shader
    // we make a simplification and use the interpolated local position of the fragment (the vector we use to sample from cubemaps normally, which also acts as
    // the surface's normal {think of a sphere}) as the viewing diretion because otherwise we would not be able to precalculate this. When it is time to sample this prefiltered map
    // to use as specular reflection for a surface, --->>> we sample it using a reflection vector (viewing direction reflected across the surface's normal). <<<--- 
  

    vec3 N = normalize(localPos);    
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);     
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            
            // in high levels of roughness, we are going to sample a high resolution environment map and output it to a very low resolution mip map. That can and will cause
            // artifacts. In order to reduce these artifacts, the code below samples from the environment map's mip map level based on roughness and pdf.
            float D   = DistributionGGX(N, H, roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

            float resolution = 1024.0; // resolution of source cubemap (per face)
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = roughness == 0.0 ? 0.0 : 0.6 * log2(saSample / saTexel); 

            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}  

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}