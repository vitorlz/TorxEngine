#version 460 core
out float FragColor;
  
in vec2 TexCoords;

uniform sampler2D gViewPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

uniform mat3 viewNormalMatrix;

uniform int screenWidth;
uniform int screenHeight;
uniform int kernelSize;
uniform float radius;
uniform float power;

vec2 noiseScale = vec2(screenWidth/4.0, screenHeight/4.0); 

void main()
{
	vec3 fragPos   = texture(gViewPosition, TexCoords).xyz;
	vec3 normal    = viewNormalMatrix * normalize(texture(gNormal, TexCoords).xyz);
	vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz; 

	vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN       = mat3(tangent, bitangent, normal);  

	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; ++i)
	{
		vec3 samplePos = TBN * samples[i]; // from tangent to view-space
		samplePos = fragPos + samplePos * radius; 

		vec4 offset = vec4(samplePos, 1.0);
		offset      = projection * offset;    
		offset.xyz /= offset.w;               
		offset.xyz  = offset.xyz * 0.5 + 0.5; 

		float sampleDepth = texture(gViewPosition, offset.xy).z; 
		 
		float bias = 0.02;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;  
	}

	occlusion = 1.0 - (occlusion / kernelSize);
	FragColor = pow(occlusion, power);
}