#include "RenderingUtil.h"
#include <glad/glad.h>
#include "../Core/Common.h"
#include "iostream"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Util/ShaderManager.h"
#include "../Util/TextureLoader.h"
#include <LinearMath/btScalar.h>
#include <random>;
#include "../Misc/Shadows.h"
#include "Util.h"

unsigned int RenderingUtil::mScreenQuadTexture;
unsigned int RenderingUtil::mBloomBrightnessTexture;
unsigned int RenderingUtil::mPointLightShadowMap;
unsigned int RenderingUtil::mPingPongFBOs[2];
unsigned int RenderingUtil::mPingPongBuffers[2];
unsigned int RenderingUtil::mUnitCubeVAO;
unsigned int RenderingUtil::mLightingFBO;
unsigned int RenderingUtil::mBlittingFBO;
unsigned int RenderingUtil::mPointLightShadowMapFBO;
unsigned int RenderingUtil::mScreenQuadVAO;
unsigned int RenderingUtil::mEnvironmentCubemap;
unsigned int RenderingUtil::mIrradianceCubemap;
unsigned int RenderingUtil::mPrefilteredEnvMap;
unsigned int RenderingUtil::mBrdfLUT;
unsigned int RenderingUtil::mDirLightShadowMapFBO;
unsigned int RenderingUtil::mDirLightShadowMap;
unsigned int RenderingUtil::mBulletDebugLinesVAO;
unsigned int RenderingUtil::mBulletDebugLinesVBO;
unsigned int RenderingUtil::mVoxelTexture;
unsigned int RenderingUtil::mVoxelizationFBO;
unsigned int RenderingUtil::mVoxelVisualizationTexture;
unsigned int RenderingUtil::mRoughnessTexture;
unsigned int RenderingUtil::mSSRTexture;
unsigned int RenderingUtil::mSSRFBO;
unsigned int RenderingUtil::mDiffuseColorTexture;
unsigned int RenderingUtil::mSSRBlurredTexture;
unsigned int RenderingUtil::mBoxBlurFBO;
std::vector<glm::vec3>  RenderingUtil::mSSAOKernel;
unsigned int RenderingUtil::mSSAONoiseTexture;
std::vector<glm::vec3> RenderingUtil::mSSAONoise;
unsigned int RenderingUtil::mSSAOTexture;
unsigned int RenderingUtil::mSSAOFBO;
unsigned int RenderingUtil::mSSAOBlurTexture;
unsigned int RenderingUtil::mSSAOBlurFBO;
unsigned int RenderingUtil::gBufferFBO;
unsigned int RenderingUtil::gPosition;
unsigned int RenderingUtil::gNormal;
unsigned int RenderingUtil::gAlbedo;
unsigned int RenderingUtil::gRMA;
unsigned int RenderingUtil::gDirLightSpacePosition;
unsigned int RenderingUtil::gEmission;
unsigned int RenderingUtil::mLightingTexture;
unsigned int RenderingUtil::gViewPosition;
unsigned int RenderingUtil::gViewNormal;
unsigned int RenderingUtil::gGameWindowFBO;
unsigned int RenderingUtil::gGameWindowTexture;
unsigned int RenderingUtil::gFinalRenderTarget{ 0 };
unsigned int RenderingUtil::gMeshId;

void RenderingUtil::Init()
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    RenderingUtil::CreateCubeVAO();
    RenderingUtil::CreateLightingFBO();
    RenderingUtil::CreateGameWindowFBO();
    RenderingUtil::CreateDirLightShadowMapFBO(DirectionalShadows::g_resolution, DirectionalShadows::g_resolution);
    RenderingUtil::CreatePointLightShadowMapFBO(OmniShadows::g_resolution, OmniShadows::g_resolution);
    RenderingUtil::CreateScreenQuadVAO();
    RenderingUtil::CreatePingPongFBOs();
    RenderingUtil::CreateSSRFBO();
    RenderingUtil::CreateSSRBoxBlurFBO();
    RenderingUtil::CreateSSAOKernel(Common::ssaoKernelSize);
    RenderingUtil::CreateSSAONoise();
    RenderingUtil::CreateSSAONoiseTexture();
    RenderingUtil::CreateSSAOFBO();
    RenderingUtil::CreateSSAOBlurFBO();
    RenderingUtil::CreateGeometryPassFBO();
  
}

float cubeVertices[] = 
{
    // positions          // normals           // texture coords
    // Back face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left

    // Front face
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left

    // Left face
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // bottom-right
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // top-left

    // Right face
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right

     // Bottom face
     -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-left
      1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-right
      1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
     -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-left

     // Top face
     -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
      1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
      1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
     -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
};

unsigned int cubeIndices[] = 
{
    // Back face
    0, 1, 2, 2, 3, 0,
    // Front face
    4, 5, 6, 6, 7, 4,
    // Left face
    8, 9, 10, 10, 11, 8,
    // Right face
    12, 13, 14, 14, 15, 12,
    // Bottom face
    16, 17, 18, 18, 19, 16,
    // Top face
    20, 21, 22, 22, 23, 20
};

float screenQuadVertices[] =
{
    // positions   // texCoords
   -1.0f,  1.0f,  0.0f, 1.0f,
   -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

   -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};

void RenderingUtil::CreateCubeVAO()
{
    unsigned int cubeVBO, cubeEBO;

    glGenVertexArrays(1, &mUnitCubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(mUnitCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void RenderingUtil::CreateScreenQuadVAO()
{
    unsigned int screenQuadVBO;

    glGenVertexArrays(1, &mScreenQuadVAO);
    glGenBuffers(1, &screenQuadVBO);

    glBindVertexArray(mScreenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void RenderingUtil::CreateLightingFBO()
{
    glGenFramebuffers(1, &mLightingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mLightingFBO);

    // FragColor texture
    
    glGenTextures(1, &mLightingTexture);
    glBindTexture(GL_TEXTURE_2D, mLightingTexture);
  
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mLightingTexture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Bloom brightness texture

    glGenTextures(1, &mBloomBrightnessTexture);
    glBindTexture(GL_TEXTURE_2D, mBloomBrightnessTexture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mBloomBrightnessTexture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    glDrawBuffers(2, attachments);

    unsigned int lightingRBO;
    glGenRenderbuffers(1, &lightingRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, lightingRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Common::SCR_WIDTH, Common::SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, lightingRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderingUtil::CreatePointLightShadowMapFBO(unsigned int shadowWidth, unsigned int shadowHeight)
{
    glGenFramebuffers(1, &mPointLightShadowMapFBO);

    glGenTextures(1, &mPointLightShadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, mPointLightShadowMap);


    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 30 , 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, mPointLightShadowMapFBO);
    // Note that we can use glFramebufferTexture to attach an entire cubemap to the framebuffer.
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mPointLightShadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingUtil::CreatePingPongFBOs() 
{
    glGenFramebuffers(2, mPingPongFBOs);
    glGenTextures(2, mPingPongBuffers);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mPingPongFBOs[i]);
        glBindTexture(GL_TEXTURE_2D, mPingPongBuffers[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPingPongBuffers[i], 0
        );
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingUtil::EquirectangularToCubemap(const char* path)
{

    static unsigned int hdrTexture = 0;
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &mEnvironmentCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvironmentCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    Shader& equiToCubemapShader = ShaderManager::GetShaderProgram("equiToCubemapShader");
    equiToCubemapShader.use();
    equiToCubemapShader.setInt("equirectangularMap", 0);
    equiToCubemapShader.setMat4("projection", captureProjection);
   
    glDeleteTextures(1, &hdrTexture);
   
    hdrTexture = TextureLoader::LoadTextureHDR(path);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 1024, 1024); // configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    glDisable(GL_CULL_FACE);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equiToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mEnvironmentCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        glBindVertexArray(mUnitCubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
   
    glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvironmentCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glEnable(GL_CULL_FACE); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingUtil::CreateIrradianceCubemap()
{
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &mIrradianceCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mIrradianceCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // As the irradiance map averages all surrounding radiance uniformly it doesn't have a lot of high frequency details, 
        // so we can store the map at a low resolution (32x32) and let OpenGL's linear filtering do most of the work.
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // this is the same thing we did for the equirectangular to cubemap function, the only difference is that we already have a cubemap 
    // (the environment map) so we don't need to convert an hdr texture to a cubemap in order to render it to a cube. We give the environment 
    // cubemap to the irradiance convolution shader, it convolutes the cubemap and then we render it to the faces of a cube and render the scene
    // 6 times, each time facing a different face of the cube and using a different face of the irradiance cubemap as the color buffer of the framebuffer
    // in order to populate the irraciance cubemap.

    Shader& irradianceConvolutionShader = ShaderManager::GetShaderProgram("irradianceConvolutionShader");
    irradianceConvolutionShader.use();
    irradianceConvolutionShader.setInt("environmentMap", 0);
    irradianceConvolutionShader.setMat4("projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvironmentCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    glDisable(GL_CULL_FACE);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceConvolutionShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mIrradianceCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(mUnitCubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    glEnable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingUtil::CreatePrefilteredEnvMap()
{
    // this is very similar to how we did the irradiance map, the difference is that we are now going to account for roughness and store
    // sequentially rougher (blurrier) reflections in the prefiltered map's mip map levels.
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 356, 356);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    
    glGenTextures(1, &mPrefilteredEnvMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mPrefilteredEnvMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 356, 356, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    Shader& prefilterConvolutionShader = ShaderManager::GetShaderProgram("prefilterConvolutionShader");
    prefilterConvolutionShader.use();
    prefilterConvolutionShader.setInt("environmentMap", 0);
    prefilterConvolutionShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvironmentCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    // here we render the cube 6 times for each of the mip map levels based on the roughness level. This way, as we increase the mip map level, the texture will become 
    // rougher /blurrier. 
    unsigned int maxMipLevels = 5;
    glDisable(GL_CULL_FACE);
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // resize framebuffer according to mip-level size.
        unsigned int mipWidth = 356 * std::pow(0.5, mip);
        unsigned int mipHeight = 356 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterConvolutionShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterConvolutionShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mPrefilteredEnvMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(mUnitCubeVAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
    }
    glEnable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingUtil::CreateBRDFIntegrationMap() 
{

    glGenTextures(1, &mBrdfLUT);

    glBindTexture(GL_TEXTURE_2D, mBrdfLUT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBrdfLUT, 0);
    glViewport(0, 0, 512, 512);

    Shader& brdfConvolutionShader = ShaderManager::GetShaderProgram("brdfConvolutionShader");
    brdfConvolutionShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindVertexArray(mScreenQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingUtil::CreateDirLightShadowMapFBO(unsigned int shadowWidth, unsigned int shadowHeight)
{
    glGenTextures(1, &mDirLightShadowMap);
    glGenFramebuffers(1, &mDirLightShadowMapFBO);

    glBindTexture(GL_TEXTURE_2D, mDirLightShadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[]{ 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, mDirLightShadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDirLightShadowMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindBuffer(GL_FRAMEBUFFER, 0);
}

void RenderingUtil::CreateBulletDebugBuffers() 
{
    glGenBuffers(1, &RenderingUtil::mBulletDebugLinesVBO);
    glGenVertexArrays(1, &RenderingUtil::mBulletDebugLinesVAO);
    glBindVertexArray(RenderingUtil::mBulletDebugLinesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderingUtil::mBulletDebugLinesVBO);
    glBufferData(GL_ARRAY_BUFFER, 10000000 * 6 * sizeof(btScalar), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(btScalar), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(btScalar), (void*)(3 * sizeof(btScalar)));
    glBindVertexArray(0);
}

void RenderingUtil::LoadNewEnvironmentMap(const char* filename)
{
    glDeleteTextures(1, &mEnvironmentCubemap);

    std::string path = "res/textures/hdr/" + std::string(filename);

    EquirectangularToCubemap(path.c_str());
}

void RenderingUtil::CreateVoxelTexture(int voxelTextureSize)
{
    glGenTextures(1, &mVoxelTexture);
    glBindTexture(GL_TEXTURE_3D, mVoxelTexture);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    GLfloat borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int texSize = voxelTextureSize;

    const std::vector <float> texture3D(4 * texSize * texSize * texSize, 0.0f);

    glTexStorage3D(GL_TEXTURE_3D, 7, GL_RGBA16F, texSize, texSize, texSize);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, texSize, texSize, texSize, 0, GL_RGBA, GL_FLOAT, texture3D.data());

    glGenerateMipmap(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void RenderingUtil::CreateSSRFBO()
{
    glGenFramebuffers(1, &mSSRFBO);
    glGenTextures(1, &mSSRTexture);
   
    glBindTexture(GL_TEXTURE_2D, mSSRTexture);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[]{ 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, mSSRFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSSRTexture, 0);

    const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        throw 0;
    }

    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingUtil::CreateSSRBoxBlurFBO()
{
    glGenFramebuffers(1, &mBoxBlurFBO);
    glGenTextures(1, &mSSRBlurredTexture);

    glBindTexture(GL_TEXTURE_2D, mSSRBlurredTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, mBoxBlurFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSSRBlurredTexture, 0);

    const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        throw 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingUtil::CreateSSAOKernel(int kernelSize)
{
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    mSSAOKernel.clear();

    for (unsigned int i = 0; i < kernelSize; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        
        // distribute more samples closer to the fragment's origin so that we can place a larger weight on occlusions closer to the actual fragment.
        float scale = float(i) / 64.0f;
        scale = Util::lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        mSSAOKernel.push_back(sample);
    }
}

void RenderingUtil::CreateSSAONoise()
{
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        mSSAONoise.push_back(noise);
    }
}

void RenderingUtil::CreateSSAONoiseTexture()
{
    glGenTextures(1, &mSSAONoiseTexture);
    glBindTexture(GL_TEXTURE_2D, mSSAONoiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &mSSAONoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void RenderingUtil::CreateSSAOFBO()
{
    glGenFramebuffers(1, &mSSAOFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mSSAOFBO);

    glGenTextures(1, &mSSAOTexture);
    glBindTexture(GL_TEXTURE_2D, mSSAOTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSSAOTexture, 0);
}

void RenderingUtil::CreateSSAOBlurFBO()
{
    glGenFramebuffers(1, &mSSAOBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mSSAOBlurFBO);
    glGenTextures(1, &mSSAOBlurTexture);
    glBindTexture(GL_TEXTURE_2D, mSSAOBlurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSSAOBlurTexture, 0);

    const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        throw 0;
    }
}

void RenderingUtil::CreateGeometryPassFBO()
{
    glGenFramebuffers(1, &gBufferFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

    // position
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // normals
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // albedo
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

    // roughness, metallic, and AO
    glGenTextures(1, &gRMA);
    glBindTexture(GL_TEXTURE_2D, gRMA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gRMA, 0);

    // emission
    glGenTextures(1, &gEmission);
    glBindTexture(GL_TEXTURE_2D, gEmission);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gEmission, 0);

    // directional light space position
    glGenTextures(1, &gDirLightSpacePosition);
    glBindTexture(GL_TEXTURE_2D, gDirLightSpacePosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gDirLightSpacePosition, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gDirLightSpacePosition, 0);


    // view position
    glGenTextures(1, &gViewPosition);
    glBindTexture(GL_TEXTURE_2D, gViewPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, gViewPosition, 0);

    // view normals
    glGenTextures(1, &gViewNormal);
    glBindTexture(GL_TEXTURE_2D, gViewNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, gViewNormal, 0);

    // mesh id's for mouse picking
    glGenTextures(1, &gMeshId);
    glBindTexture(GL_TEXTURE_2D, gMeshId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT8, GL_TEXTURE_2D, gMeshId, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int attachments[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,  GL_COLOR_ATTACHMENT6,  GL_COLOR_ATTACHMENT7};
    glDrawBuffers(8, attachments);

    unsigned int gBufferRBO;
    glGenRenderbuffers(1, &gBufferRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, gBufferRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Common::SCR_WIDTH, Common::SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gBufferRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingUtil::DeleteTexture(unsigned int textureId) 
{
    glDeleteTextures(1, &textureId);
}

void RenderingUtil::CreateGameWindowFBO()
{
    glGenFramebuffers(1, &gGameWindowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gGameWindowFBO);

    glGenTextures(1, &gGameWindowTexture);
    glBindTexture(GL_TEXTURE_2D, gGameWindowTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gGameWindowTexture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int gameWindowRBO;
    glGenRenderbuffers(1, &gameWindowRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, gameWindowRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Common::SCR_WIDTH, Common::SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gameWindowRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}