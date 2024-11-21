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
#include "../Components/CPlayer.h"
#include "../Components/CMesh.h"
#include "../Components/CRigidBody.h"
#include "../Util/ShaderManager.h"
#include "../Util/TextureLoader.h"
#include "../Util/Util.h"
#include "../Rendering/RenderingUtil.h"
#include "../AssetLoading/AssetManager.h"
#include "../Physics/Raycast.h"
#include "../Physics/BulletDebugDrawer.h"
#include "../UI/UI.h"
#include "../Editor/Editor.h"

extern Coordinator ecs;

void RenderSystem::Init() 
{
    RenderingUtil::Init();
}

const int MAX_OMNISHADOWS = 10;

// PBR testing 
glm::vec3 lightPositions[] = {
    glm::vec3(-10.0f,  10.0f, 10.0f),
    glm::vec3(10.0f,  10.0f, 10.0f),
    glm::vec3(-10.0f, -10.0f, 10.0f),
    glm::vec3(10.0f, -10.0f, 10.0f),
};
glm::vec3 lightColors[] = {
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f)
};
int nrRows = 7;
int nrColumns = 7;
float spacing = 0.4;
// PBR testing 
glm::mat4 dirLightSpaceMatrix;
void RenderSystem::Update(float deltaTime)
{
    // ------------------------- DIRECTIONAL SHADOWS PASS ---------------------------------------


    // this probably should not be here. For something to go through the render system it has to have a model. This means that we have to add a model to 
    // the directional light and scale it to 0 if we want it to go through the process below.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
    bool dirtyDirLightFound = false;
    glm::vec3 dirLightPos;

    for (const auto& entity : mEntities)
    {
        if (ecs.HasComponent<CLight>(entity))
        {
            auto& light = ecs.GetComponent<CLight>(entity);
            
            if (light.shadowCaster && light.type == DIRECTIONAL)
            {
                auto& transform = ecs.GetComponent<CTransform>(entity);
               
                dirLightPos = transform.position;

                if (light.isDirty)
                {
                    dirtyDirLightFound = true;
                }
            }
        }
    }

    if (dirtyDirLightFound)
    {
        float near_plane = 0.1f, far_plane = 40.0f;
        glm::mat4 lightProjection = glm::ortho(-17.0f, 17.0f, -17.0f, 17.0f, near_plane, far_plane);

        glm::mat4 lightView = glm::lookAt(dirLightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        dirLightSpaceMatrix = lightProjection * lightView;

        glViewport(0, 0, 2048, 2048);
        glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::mDirLightShadowMapFBO);

        glClear(GL_DEPTH_BUFFER_BIT);

        // we need to send the directional light space matrix to the lighting shader so that we can calculate the texcoords of the current fragment
        // to sample from the shadow map.

        Shader& dirShadowMapShader = ShaderManager::GetShaderProgram("dirShadowMapShader");

        dirShadowMapShader.use();

        dirShadowMapShader.setMat4("lightSpaceMatrix", dirLightSpaceMatrix);

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

            dirShadowMapShader.setMat4("model", model);

            std::cout << "stuff rendered to dir shadow map" << "\n";
            model3d.model.Draw(dirShadowMapShader);
        }
    }

    // ------------------------- OMNIDIRECTIONAL SHADOWS PASS -----------------------------------

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);

    int omniShadowCasters = 0;
    std::vector<glm::vec3> lightPos;
    lightPos.reserve(MAX_OMNISHADOWS);
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

            if (light.shadowCaster && light.type == POINT) 
            {
                auto& transform = ecs.GetComponent<CTransform>(entity);
                lightPos.push_back(transform.position + light.offset);
                pointFar.push_back(light.radius);
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
        glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::mPointLightShadowMapFBO);

        glClear(GL_DEPTH_BUFFER_BIT);

        Shader& pointShadowMapShader = ShaderManager::GetShaderProgram("pointShadowMapShader");

        pointShadowMapShader.use();

        pointShadowMapShader.setInt("omniShadowCasters", omniShadowCasters);

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

            pointShadowMapShader.setMat4("shadowMatrices[" + std::to_string(j) + "]", shadowTransforms[j]);
        }

        for (int i = 0; i < omniShadowCasters; i++)
        {
            pointShadowMapShader.setFloat("far_plane", pointFar[i]);
            pointShadowMapShader.setVec3("lightPos", lightPos[i]);

            for (const auto& entity : mEntities)
            {

                if (ecs.HasComponent<CLight>(entity) || (!ecs.HasComponent<CModel>(entity) && !ecs.HasComponent<CMesh>(entity)))
                {
                    continue;
                }            

                auto& transform = ecs.GetComponent<CTransform>(entity);
               
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, transform.position);

                model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0, 0.0));
                model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0, 0.0));
                model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0, 1.0));
                model = glm::scale(model, transform.scale);

                pointShadowMapShader.setMat4("model", model);

                if (ecs.HasComponent<CModel>(entity))
                {
                    auto& modelComponent = ecs.GetComponent<CModel>(entity);
                    modelComponent.model.Draw(pointShadowMapShader);
                }
                else
                {
                    auto& meshComponent = ecs.GetComponent<CMesh>(entity);
                    meshComponent.mesh.Draw(pointShadowMapShader);
                    
                }
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

    glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::mMsFBO);
    
    // set both color buffer attachments as the draw buffers before clearing them, otherwise only the first color attachment will get cleared.
    // This would mean that the color buffer which we render the bloom brightness texture onto would not get cleared and the bloom would effect would
    // just accumulate over frames.
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(
        glm::radians(45.0f), (float)Window::screenWidth / (float)Window::screenHeight, 0.1f, 200.0f);

    // get player
    Entity playerEntity{};
    for (const auto& entity : mEntities)
    {
        if (ecs.HasComponent<CPlayer>(entity))
        {
            playerEntity = entity;
        }
    }

    auto& player = ecs.GetComponent<CPlayer>(playerEntity);

    Common::playerViewMatrix = player.viewMatrix;

    if (UI::isOpen)
    {
        Raycast::calculateMouseRaycast(projection* player.viewMatrix);
    }

    Shader& pbrModelTestShader = ShaderManager::GetShaderProgram("pbrModelTestShader");
    pbrModelTestShader.use();

    pbrModelTestShader.setMat4("view", player.viewMatrix);
    pbrModelTestShader.setMat4("projection", projection);
    pbrModelTestShader.setVec3("camPos", ecs.GetComponent<CTransform>(playerEntity).position);
    pbrModelTestShader.setBool("showNormals", Common::normalsDebug);
    pbrModelTestShader.setBool("worldPosDebug", Common::worldPosDebug);
    pbrModelTestShader.setBool("albedoDebug", Common::albedoDebug);
    pbrModelTestShader.setBool("roughnessDebug", Common::roughnessDebug);
    pbrModelTestShader.setBool("metallicDebug", Common::metallicDebug);
    pbrModelTestShader.setBool("aoDebug", Common::aoDebug);
    pbrModelTestShader.setBool("emissionDebug", Common::emissionDebug);
    pbrModelTestShader.setBool("bloom", Common::bloomOn);
    pbrModelTestShader.setInt("irradianceMap", 6);
    pbrModelTestShader.setInt("prefilterMap", 7);
    pbrModelTestShader.setInt("brdfLUT", 8);
    pbrModelTestShader.setInt("dirShadowMap", 9);
    pbrModelTestShader.setMat4("dirLightSpaceMatrix", dirLightSpaceMatrix);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, RenderingUtil::mIrradianceCubemap);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, RenderingUtil::mPrefilteredEnvMap);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, RenderingUtil::mBrdfLUT);
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, RenderingUtil::mDirLightShadowMap);

    for (const auto& entity : mEntities) 
    {

        auto& transform = ecs.GetComponent<CTransform>(entity);

        if (ecs.HasComponent<CLight>(entity) && Common::lightPosDebug)
        {
            auto& light = ecs.GetComponent<CLight>(entity);

            Shader& solidColorShader = ShaderManager::GetShaderProgram("solidColorShader");

            solidColorShader.use();

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position + light.offset);
            model = glm::scale(model, glm::vec3(0.1f));

            solidColorShader.setMat4("projection", projection);
            solidColorShader.setMat4("view", player.viewMatrix);
            solidColorShader.setMat4("model", model);
            solidColorShader.setVec3("color", light.color * light.strength);

            Util::renderSphere();
        }

        if (!ecs.HasComponent<CModel>(entity) && !ecs.HasComponent<CMesh>(entity))
        {
            continue;
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);

        glm::mat4 rotMatrix = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(transform.rotation.x), glm::radians(transform.rotation.y), glm::radians(transform.rotation.z))));

        model *= rotMatrix;

        model = glm::scale(model, transform.scale);

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
        
        pbrModelTestShader.use();

        if (omniShadowCasters != 0) 
        {
            glActiveTexture(GL_TEXTURE10);
            glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, RenderingUtil::mPointLightShadowMap);
            pbrModelTestShader.setInt("pointShadowMap", 10);

            glUniform1fv(glGetUniformLocation(pbrModelTestShader.ID, "point_far_plane"), omniShadowCasters, pointFar.data());
        }

        pbrModelTestShader.setMat4("model", model);
        pbrModelTestShader.setMat3("normalMatrix", normalMatrix);

        if (ecs.HasComponent<CModel>(entity))
        {
            auto& model3d = ecs.GetComponent<CModel>(entity);
            pbrModelTestShader.setVec2("textureScalingFactor", glm::vec2(1.0f));
            pbrModelTestShader.setBool("hasAOTexture", model3d.hasAOTexture);
            model3d.model.Draw(pbrModelTestShader);
        }
        else
        {
            glDisable(GL_CULL_FACE);
            auto& meshComponent = ecs.GetComponent<CMesh>(entity);
            pbrModelTestShader.setVec2("textureScalingFactor", meshComponent.textureScalingFactor);
            meshComponent.mesh.Draw(pbrModelTestShader);
            glEnable(GL_CULL_FACE);
        }
    }

   if (Common::bulletLinesDebug)
   {
        Shader& lineDebugShader = ShaderManager::GetShaderProgram("lineDebugShader");

        lineDebugShader.use();

        lineDebugShader.setMat4("projection", projection);
        lineDebugShader.setMat4("view", player.viewMatrix);
       
        BulletDebugDrawer::drawLines();
   }

    // ----------------------------- PBR TESTING -----------------------------------------------------------------------------------------

    if (Common::pbrDemonstration)
    {
        glDrawBuffers(1, attachments);
        glEnable(GL_DEPTH_TEST);

        Shader& pbrLightingTestShader = ShaderManager::GetShaderProgram("pbrLightingTestShader");
        pbrLightingTestShader.use();
        pbrLightingTestShader.setMat4("view", player.viewMatrix);
        pbrLightingTestShader.setVec3("camPos", ecs.GetComponent<CTransform>(playerEntity).position);
        pbrLightingTestShader.setVec3("albedo", 0.5f, 0.0f, 0.0f);
        pbrLightingTestShader.setFloat("ao", 1.0f);
        pbrLightingTestShader.setInt("irradianceMap", 2);
        pbrLightingTestShader.setInt("prefilterMap", 3);
        pbrLightingTestShader.setInt("brdfLUT", 4);
       
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, RenderingUtil::mIrradianceCubemap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, RenderingUtil::mPrefilteredEnvMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mBrdfLUT);

        pbrLightingTestShader.setMat4("projection", projection);

        // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
        glm::mat4 model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; ++row)
        {
            pbrLightingTestShader.setFloat("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                pbrLightingTestShader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (col - (nrColumns / 2)) * spacing,
                    (row - (nrRows / 2)) * spacing,
                    0.0f
                ));
                model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.15f));
                pbrLightingTestShader.setMat4("model", model);
                pbrLightingTestShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
                Util::renderSphere();
            }
        }
    }

    // ---------------------------- SKYBOX PASS ---------------------------------------
    glDrawBuffers(1, attachments);
    
    if (Common::wireframeDebug) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDisable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);

    Shader& skyBoxShader = ShaderManager::GetShaderProgram("cubemapShader");

    skyBoxShader.use();

    skyBoxShader.setMat4("projection", projection);
    skyBoxShader.setMat4("view", glm::mat4(glm::mat3(player.viewMatrix)));
    glBindVertexArray(RenderingUtil::mUnitCubeVAO);
    skyBoxShader.setInt("skybox", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, RenderingUtil::mEnvironmentCubemap);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthFunc(GL_LESS);

    // ---------------------------------- BLITTING ---------------------------------------

    // Set up to blit from the first color buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, RenderingUtil::mMsFBO);
    glReadBuffer(GL_COLOR_ATTACHMENT0);  // Read from the first color attachment

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RenderingUtil::mBlittingFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);  // Draw to the first color attachment

    glBlitFramebuffer(0, 0, Window::screenWidth, Window::screenHeight, 0, 0, Window::screenWidth, Window::screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Condition to do bloom

    if (Common::bloomOn) {
        // Now blit from the second color buffer
        glReadBuffer(GL_COLOR_ATTACHMENT1);  // Read from the second color attachment

        glDrawBuffer(GL_COLOR_ATTACHMENT1);  // Draw to the second color attachment

        glBlitFramebuffer(0, 0, Window::screenWidth, Window::screenHeight, 0, 0, Window::screenWidth, Window::screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // ---------------------------------- BLOOM PASS -----------------------------------------------------------------

    bool horizontal = true, first_iteration = true;

    if (Common::bloomOn)
    {
        Shader& blurShader = ShaderManager::GetShaderProgram("blurShader");
        blurShader.use();

        int kernelSize = Common::bloomKernelSize;
        float stdDeviation = Common::bloomStdDeviation;
        float intervalMultiplier = Common::bloomIntervalMultiplier;

        blurShader.setFloat("kernelSize", Common::bloomKernelSize);

        Common::bloomWeights = Util::gaussian_weights(kernelSize, stdDeviation, intervalMultiplier);

        std::vector<float> weights = Common::bloomWeights;

        glUniform1fv(glGetUniformLocation(blurShader.ID, "weight"), kernelSize, weights.data());

        glActiveTexture(GL_TEXTURE0);
        blurShader.setInt("image", 0);

        for (unsigned int i = 0; i < kernelSize * 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::mPingPongFBOs[horizontal]);
            blurShader.setInt("horizontal", horizontal);
            glBindTexture(
                GL_TEXTURE_2D, first_iteration ? RenderingUtil::mBloomBrightnessTexture : RenderingUtil::mPingPongBuffers[!horizontal]
            );

            glBindVertexArray(RenderingUtil::mScreenQuadVAO);
            glDisable(GL_DEPTH_TEST);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
    }

    // ------------------------------ POST PROCESSING PASS -----------------------------------------------------------
    
    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Shader& postProcessingShader = ShaderManager::GetShaderProgram("postProcessingShader");
    postProcessingShader.use();
    postProcessingShader.setBool("showNormals", Common::normalsDebug);
    postProcessingShader.setBool("worldPosDebug", Common::worldPosDebug);
    postProcessingShader.setBool("albedoDebug", Common::albedoDebug);
    postProcessingShader.setBool("roughnessDebug", Common::roughnessDebug);
    postProcessingShader.setBool("metallicDebug", Common::metallicDebug);
    postProcessingShader.setFloat("exposure", Common::exposure);
    postProcessingShader.setBool("reinhard", Common::reinhard);
    postProcessingShader.setBool("uncharted2", Common::uncharted);
    postProcessingShader.setBool("ACES", Common::aces);
    postProcessingShader.setBool("bloom", Common::bloomOn);
   
    glBindVertexArray(RenderingUtil::mScreenQuadVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RenderingUtil::mScreenQuadTexture);

    if (Common::bloomOn) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mPingPongBuffers[(Common::bloomKernelSize * 2) % 2 ? !horizontal : horizontal]);
    }

    postProcessingShader.setInt("screenQuadTexture", 0);
    postProcessingShader.setInt("bloomBlurTexture", 1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}