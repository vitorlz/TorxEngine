#include "RenderingUtil.h"
#include "glad/glad.h"
#include "../Core/Common.h"
#include "iostream"

unsigned int RenderingUtil::mScreenQuadTexture;
unsigned int RenderingUtil::mBloomBrightnessTexture;
unsigned int RenderingUtil::mPointLightShadowMap;
unsigned int RenderingUtil::mPingPongFBOs[2];
unsigned int RenderingUtil::mPingPongBuffers[2];

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

unsigned int RenderingUtil::CreateCubeVAO()
{
    unsigned int cubeVBO, cubeEBO, cubeVAO;

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

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

    return cubeVAO;
}

unsigned int RenderingUtil::CreateScreenQuadVAO()
{
    unsigned int screenQuadVAO, screenQuadVBO;

    glGenVertexArrays(1, &screenQuadVAO);
    glGenBuffers(1, &screenQuadVBO);

    glBindVertexArray(screenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return screenQuadVAO;
}

unsigned int RenderingUtil::CreateMSAAFBO()
{
    unsigned int msFBO;
    glGenFramebuffers(1, &msFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, msFBO);

    // create multisampled texture to use as color buffer attachment of multisampled framebuffer

    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBuffers[i]);
        // We are using a floating point texture (RGB16F) so that we can store color values greater than 1 (high dynamic range)
        // we can then set the color values of the lights as high as we want (conveying the true brightness of the lights).
        // We then convert these values back to the 0.0 to 1.0 range (low dynamic range) in the final shader (the shader used to render stuff to the screen quad)
        // using a process called tone mapping (for which we have many different algorithms).
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, GL_TRUE);

        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, colorBuffers[i], 0);
    }

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    // We have to tell opengl that we are drawing to two color buffers when using this framebuffer, otherwise opengl will only render to the first
    // color attachments and ignore the others.

    // -------- UNCOMMENT THIS WHEN IT IS TIME TO DO BLOOM ---------------

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    glDrawBuffers(2, attachments);

    // create multisampled depth and stencil buffers as attachments of multisampled framebuffer

    unsigned int msRbo;
    glGenRenderbuffers(1, &msRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, msRbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, Common::SCR_WIDTH, Common::SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msRbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return msFBO;
}

unsigned int RenderingUtil::CreateBlittingFBO()
{
    // SCREEN QUAD FRAMEBUFFER (WE BLIT BOTH THE MULTISAMPLED SCENE AND THE MULTISAMPLED BLUR BRIGHTNESS TEXTURE INTO THIS FRAMEBUFFER)
    unsigned int blittingFBO;
    glGenFramebuffers(1, &blittingFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, blittingFBO);

    // SCREEN QUAD TEXTURE

   
    glGenTextures(1, &mScreenQuadTexture);
    glBindTexture(GL_TEXTURE_2D, mScreenQuadTexture);

    // This has to be a floating point framebuffer otherwise we would not have sufficient color precision after gamma correcting and we would see artifacts
    // for lower color values. We need to do this if we want to gamma correct in the final pass (post processing / screen quad pass).
    // Gamma correction amplifies differences in lower color values, which can reveal artifacts. Gamma correction amplifies differences in lower color values, 
    // which can reveal artifacts. Floating-point colors in shaders prevent these artifacts because of higher precision.

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    // attach the texture object as the color attachment / buffer of the currently bound framebuffer.

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mScreenQuadTexture, 0);

    // BLUR TEXTURE 

    glGenTextures(1, &mBloomBrightnessTexture);
    glBindTexture(GL_TEXTURE_2D, mBloomBrightnessTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mBloomBrightnessTexture, 0);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    //We also want to make sure OpenGL is able to do depth testing (and optionally stencil testing) so we have to make
    // sure to add a depth (and stencil) attachment to the framebuffer. Since we'll only be sampling the color buffer and 
    // not the other buffers we can create a renderbuffer object for this purpose. Remember that we can only write
    // to renderbuffer objects, we cannot read from them. We are going to use one renderbuffer object as both the depth and stencil
    // buffers.

    unsigned int screenRbo;
    glGenRenderbuffers(1, &screenRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, screenRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Common::SCR_WIDTH, Common::SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // now, to complete the framebuffer, we attach the renderbuffer object to both the depth and stencil attachment of the framebuffer:

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screenRbo);

    // check if the currently bound framebuffer is complete and if it's not, print an error message

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    // make sure to unbind the framebuffer to make sure we are not accidentally rendering to the wrong framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return blittingFBO;
}

unsigned int RenderingUtil::CreatePointLightShadowMapFBO(unsigned int shadowWidth, unsigned int shadowHeight)
{

    unsigned int pointLightShadowMapFBO;
    glGenFramebuffers(1, &pointLightShadowMapFBO);

    glGenTextures(1, &mPointLightShadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, mPointLightShadowMap);

    // CUBE MAPS REQUIRE THE SAME WIDTH AND HEIGHT FOR THE TEXTURE IMAGE SIZE OF EACH FACE OF THE CUBE. SO SHADOW_WIDTH AND SHADOW_HEIGHT HAVE TO BE THE SAME

   
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 2 * 24, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, pointLightShadowMapFBO);
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

    return pointLightShadowMapFBO;
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

unsigned int RenderingUtil::GetScreenQuadTexture() 
{
    return mScreenQuadTexture;
}

unsigned int RenderingUtil::GetBloomBrightnessTexture() 
{
    return mBloomBrightnessTexture;
}

unsigned int RenderingUtil::GetPointLightShadowMap()
{
    return mPointLightShadowMap;
}