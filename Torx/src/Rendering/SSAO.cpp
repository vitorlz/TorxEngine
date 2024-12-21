#include "SSAO.h"

#include <random>
#include <glad/glad.h>



SSAO::SSAO(int kernelSize)
{
    CreateSSAOKernelAndNoise(kernelSize);
    CreateSSAONoiseTexture();
}

void SSAO::CreateSSAOKernelAndNoise(int kernelSize)
{
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0); 
    std::default_random_engine generator;
   
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        m_SSAOKernel.push_back(sample);
    }

    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        m_SSAONoise.push_back(noise);
    }
}

void SSAO::CreateSSAONoiseTexture()
{
    glGenTextures(1, &m_SSAONoiseTexture);
    glBindTexture(GL_TEXTURE_2D, m_SSAONoiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &m_SSAONoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
