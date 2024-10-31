#include "RenderSystem.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "../Util/Window.h"
#include "../Core/Common.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CTransform.h"
#include "../Components/CMesh.h"
#include "../Components/CLight.h"
#include "../Util/ShaderManager.h"
#include "../Util/TextureLoader.h"
#include "../Rendering/RenderingUtil.h"


extern Coordinator ecs;

void RenderSystem::Init() 
{
    mCubemapID = TextureLoader::LoadCubeMap(
        "res/textures/cubemaps/right.jpg",
        "res/textures/cubemaps/left.jpg",
        "res/textures/cubemaps/top.jpg",
        "res/textures/cubemaps/bottom.jpg",
        "res/textures/cubemaps/front.jpg",
        "res/textures/cubemaps/back.jpg"
    );

    mCubeVAO = RenderingUtil::CreateCubeVAO();
    mMsFBO = RenderingUtil::CreateMSAAFBO();
    mBlittingFBO = RenderingUtil::CreateBlittingFBO();
    mScreenQuadVAO = RenderingUtil::CreateScreenQuadVAO();
    mLightingShader = ShaderManager::GetShaderProgram("lightingShader");
    mSolidColorShader = ShaderManager::GetShaderProgram("solidColorShader");
    mSkyBoxShader = ShaderManager::GetShaderProgram("cubemapShader");
    mPostProcessingShader = ShaderManager::GetShaderProgram("postProcessingShader");
    mScreenQuadTexture = RenderingUtil::GetScreenQuadTexture();
}

void RenderSystem::Update(float deltaTime, Camera& camera)
{

   

    // ------------------------- LIGHTING PASS -----------------------------------

    if (Common::wireframeDebug)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mMsFBO);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, Window::screenWidth, Window::screenHeight);

    mLightingShader.use();

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(
        glm::radians(camera.Zoom), (float)Window::screenWidth / (float)Window::screenHeight, 0.1f, 100.0f);

    glEnable(GL_CULL_FACE);

    for (const auto& entity : mEntities) 
    {
        auto& transform = ecs.GetComponent<CTransform>(entity);
        const auto& mesh = ecs.GetComponent<CMesh>(entity);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
     
        model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0, 0.0));
        model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0, 1.0));
        model = glm::scale(model, transform.scale);

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));

        if (ecs.HasComponent<CLight>(entity)) {

            auto& light = ecs.GetComponent<CLight>(entity);

            mSolidColorShader.use(); 
           
            mSolidColorShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            mSolidColorShader.setMat4("view", view);
            mSolidColorShader.setMat4("model", model);
            mSolidColorShader.setVec3("color", ecs.GetComponent<CLight>(entity).diffuse * 1.5f);

            for (Mesh mesh : mesh.meshes) {
                mesh.Draw(mSolidColorShader);
            }

            continue;
        }
        
        mLightingShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        mLightingShader.setMat4("view", view);
        mLightingShader.setMat4("model", model);
        mLightingShader.setMat3("normalMatrix", normalMatrix);
        mLightingShader.setBool("showNormals", Common::normalsDebug);
        mLightingShader.setBool("worldPosDebug", Common::worldPosDebug);


        mLightingShader.setVec3("cameraPos", camera.Position);
        mLightingShader.setVec3("cameraFront", camera.Front);

        for (Mesh mesh : mesh.meshes) {
            mesh.Draw(mLightingShader);
        }
    }
    
    // ---------------------------- SKYBOX PASS ---------------------------------------

    if (Common::wireframeDebug) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDisable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);

    mSkyBoxShader.use();

    view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

    mSkyBoxShader.setMat4("projection", projection);
    mSkyBoxShader.setMat4("view", view);
    glBindVertexArray(mCubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemapID);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthFunc(GL_LESS);

    // ---------------------------------- BLITTING ---------------------------------------

    // Set up to blit from the first color buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mMsFBO);
   // glReadBuffer(GL_COLOR_ATTACHMENT0);  // Read from the first color attachment

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBlittingFBO);
    //glDrawBuffer(GL_COLOR_ATTACHMENT0);  // Draw to the first color attachment

    glBlitFramebuffer(0, 0, Window::screenWidth, Window::screenHeight, 0, 0, Window::screenWidth, Window::screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Condition to do bloom

    if (false) {
        // Now blit from the second color buffer
        glReadBuffer(GL_COLOR_ATTACHMENT1);  // Read from the second color attachment

        glDrawBuffer(GL_COLOR_ATTACHMENT1);  // Draw to the second color attachment

        glBlitFramebuffer(0, 0, Window::screenWidth, Window::screenHeight, 0, 0, Window::screenWidth, Window::screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // ------------------------------ POST PROCESSING PASS ---------------------

    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mPostProcessingShader.use();
    mPostProcessingShader.setBool("showNormals", Common::normalsDebug);
    mPostProcessingShader.setBool("worldPosDebug", Common::worldPosDebug);
    mPostProcessingShader.setFloat("exposure", Common::exposure);
    mPostProcessingShader.setBool("reinhard", Common::reinhard);
    mPostProcessingShader.setBool("uncharted2", Common::uncharted);
    mPostProcessingShader.setBool("ACES", Common::aces);
   
    glBindVertexArray(mScreenQuadVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mScreenQuadTexture);

    /*  if (bloom) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffers[(kernelSize * 2) % 2 ? !horizontal : horizontal]);
    }*/

    mPostProcessingShader.setInt("screenQuadTexture", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}