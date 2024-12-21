#pragma once

#include <glm/glm.hpp>
#include <vector>

class SSAO
{
public:
	SSAO(int kernelSize);
	std::vector<glm::vec3> m_SSAOKernel;
	unsigned int m_SSAONoiseTexture;

private:
	void CreateSSAOKernelAndNoise(int kernelSize);
	void CreateSSAONoiseTexture();
	std::vector<glm::vec3> m_SSAONoise;
};