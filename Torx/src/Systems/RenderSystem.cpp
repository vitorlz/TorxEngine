#include "RenderSystem.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <iostream>
#include <string>

#include "../Util/Window.h"
#include "../Core/Common.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CTransform.h"
#include "../Components/CLight.h"
#include "../Components/CModel.h"
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
    mPointLightShadowMapFBO = RenderingUtil::CreatePointLightShadowMapFBO(1024, 1024);
    mPointLightShadowMap = RenderingUtil::GetPointLightShadowMap();
    mScreenQuadVAO = RenderingUtil::CreateScreenQuadVAO();
    mScreenQuadTexture = RenderingUtil::GetScreenQuadTexture();
}

const int MAX_OMNISHADOWS = 10;

void RenderSystem::Update(float deltaTime, Camera& camera)
{
    // ------------------------- OMNIDIRECTIONAL SHADOWS PASS -----------------------------------

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);

    int omniShadowCasters = 0;
    glm::vec3 lightPos[MAX_OMNISHADOWS]{};
    glm::mat4 shadowProj;
    std::vector<float> pointFar;
    pointFar.reserve(MAX_OMNISHADOWS);
    float pointNear = 1.0f;
    std::vector<glm::mat4> shadowTransforms;
    bool dirtyLightFound = false;

    // get light position, radius, and increase shadowcaster counter
    for (const auto& entity : mEntities)
    {
        if (ecs.HasComponent<CLight>(entity))
        {
            auto& light = ecs.GetComponent<CLight>(entity);

            if (light.shadowCaster) 
            {
                auto& transform = ecs.GetComponent<CTransform>(entity);
                lightPos[omniShadowCasters] = transform.position;
                pointFar[omniShadowCasters] = light.radius;
                omniShadowCasters++;

                if (light.isDirty)
                {
                    dirtyLightFound = true;
                }
            }
        }
    }
    
    if (dirtyLightFound)
    {

        std::cout << "shadow map rendered \n";
        glViewport(0, 0, ((float)1024), ((float)1024));
        glBindFramebuffer(GL_FRAMEBUFFER, mPointLightShadowMapFBO);

        glClear(GL_DEPTH_BUFFER_BIT);

        mPointShadowMapShader = ShaderManager::GetShaderProgram("pointShadowMapShader");

        mPointShadowMapShader.use();

        mPointShadowMapShader.setInt("omniShadowCasters", omniShadowCasters);

        for (int i = 0; i < omniShadowCasters; i++)
        {
            shadowProj = glm::perspective(glm::radians(90.0f), ((float)1024) / ((float)1024), pointNear, pointFar[i]);

            shadowTransforms.push_back(shadowProj *
                glm::lookAt(lightPos[i], lightPos[i] + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(shadowProj *
                glm::lookAt(lightPos[i], lightPos[i] + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(shadowProj *
                glm::lookAt(lightPos[i], lightPos[i] + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
            shadowTransforms.push_back(shadowProj *
                glm::lookAt(lightPos[i], lightPos[i] + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
            shadowTransforms.push_back(shadowProj *
                glm::lookAt(lightPos[i], lightPos[i] + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(shadowProj *
                glm::lookAt(lightPos[i], lightPos[i] + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
        }

        for (unsigned int j = 0; j < (omniShadowCasters * 6); ++j) {

            mPointShadowMapShader.setMat4("shadowMatrices[" + std::to_string(j) + "]", shadowTransforms[j]);
        }

        for (int i = 0; i < omniShadowCasters; i++)
        {

            mPointShadowMapShader.setFloat("far_plane", pointFar[i]);
            mPointShadowMapShader.setVec3("lightPos", lightPos[i]);

            for (const auto& entity : mEntities)
            {
                auto& transform = ecs.GetComponent<CTransform>(entity);
                auto& model3d = ecs.GetComponent<CModel>(entity);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, transform.position);

                model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0, 0.0));
                model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0, 0.0));
                model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0, 1.0));
                model = glm::scale(model, transform.scale);

                mPointShadowMapShader.setMat4("model", model);

                model3d.model.Draw(mPointShadowMapShader);
            }
        }
    }

    // ------------------------- LIGHTING PASS -----------------------------------

    glCullFace(GL_BACK);

    if (Common::wireframeDebug)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glViewport(0, 0, Window::screenWidth, Window::screenHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, mMsFBO);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
  
    mLightingShader = ShaderManager::GetShaderProgram("lightingShader");

    mLightingShader.use();

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(
        glm::radians(camera.Zoom), (float)Window::screenWidth / (float)Window::screenHeight, 0.1f, 200.0f);

    for (const auto& entity : mEntities) 
    {
        auto& transform = ecs.GetComponent<CTransform>(entity);
        auto& model3d = ecs.GetComponent<CModel>(entity);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
     
        model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0, 0.0));
        model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0, 1.0));
        model = glm::scale(model, transform.scale);

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));

        if (ecs.HasComponent<CLight>(entity)) 
        {
            auto& light = ecs.GetComponent<CLight>(entity);

            mSolidColorShader = ShaderManager::GetShaderProgram("solidColorShader");

            mSolidColorShader.use(); 
           
            mSolidColorShader.setMat4("projection", projection);
            mSolidColorShader.setMat4("view", view);
            mSolidColorShader.setMat4("model", model);
            mSolidColorShader.setVec3("color", ecs.GetComponent<CLight>(entity).diffuse * 1.5f);

            model3d.model.Draw(mSolidColorShader);
        }

        mLightingShader.use();
        
        if (omniShadowCasters != 0) 
        {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, mPointLightShadowMap);

            mLightingShader.setInt("pointShadowMap", 2);

            glUniform1fv(glGetUniformLocation(mLightingShader.ID, "point_far_plane"), omniShadowCasters, pointFar.data());
        }
        
        mLightingShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        mLightingShader.setMat4("view", view);
        mLightingShader.setMat4("model", model);
        mLightingShader.setMat3("normalMatrix", normalMatrix);
        mLightingShader.setBool("showNormals", Common::normalsDebug);
        mLightingShader.setBool("worldPosDebug", Common::worldPosDebug);

        mLightingShader.setVec3("cameraPos", camera.Position);
        mLightingShader.setVec3("cameraFront", camera.Front);

        model3d.model.Draw(mLightingShader);
    }
    
    // ---------------------------- SKYBOX PASS ---------------------------------------

    if (Common::wireframeDebug) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDisable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);

    mSkyBoxShader = ShaderManager::GetShaderProgram("cubemapShader");

    mSkyBoxShader.use();

    //mSkyBoxShader.setInt("skybox", 6);
   // glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_CUBE_MAP, mPointLightShadowMap);

    view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

    mSkyBoxShader.setMat4("projection", projection);
    mSkyBoxShader.setMat4("view", view);
    glBindVertexArray(mCubeVAO);
    mSkyBoxShader.setInt("skybox", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, mPointLightShadowMap);
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

    mPostProcessingShader = ShaderManager::GetShaderProgram("postProcessingShader");
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