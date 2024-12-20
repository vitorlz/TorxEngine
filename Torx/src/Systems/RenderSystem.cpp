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
    RenderingUtil::CreateVoxelTexture(Common::voxelGridDimensions);
}

glm::mat4 dirLightSpaceMatrix;
void RenderSystem::Update(float deltaTime)
{
    // ------------------------- DIRECTIONAL SHADOWS PASS ---------------------------------------

    // this probably should not be here. For something to go through the render system it has to have a model. This means that we have to add a model to 
    // the directional light and scale it to 0 if we want it to go through the process below.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
    std::vector<Entity> dirLightShadowEntities;
    dirLightShadowEntities.resize(10);

    for (const auto& entity : mEntities)
    {
        if (ecs.HasComponent<CLight>(entity))
        {
            auto& light = ecs.GetComponent<CLight>(entity);
            
            if (light.shadowCaster && light.type == DIRECTIONAL)
            {
                dirLightShadowEntities.push_back(entity);
            }
        }
    }

    for (Entity lightEntity : dirLightShadowEntities)
    {
        auto& transform = ecs.GetComponent<CTransform>(lightEntity);

        float near_plane = 0.1f, far_plane = 40.0f;
        glm::mat4 lightProjection = glm::ortho(-17.0f, 17.0f, -17.0f, 17.0f, near_plane, far_plane);

        glm::mat4 lightView = glm::lookAt(transform.position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

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

            if (!ecs.HasComponent<CModel>(entity) && !ecs.HasComponent<CMesh>(entity))
            {
                continue;
            }

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);

            glm::mat4 rotMatrix = glm::mat4_cast(transform.rotation);

            model *= rotMatrix;

            model = glm::scale(model, transform.scale);

            dirShadowMapShader.setMat4("model", model);
            if (ecs.HasComponent<CModel>(entity))
            {
                auto& modelComponent = ecs.GetComponent<CModel>(entity);
                modelComponent.model.Draw(dirShadowMapShader);
            }
            else
            {
                auto& meshComponent = ecs.GetComponent<CMesh>(entity);
                meshComponent.mesh.Draw(dirShadowMapShader);

            }
        }
    }

    // ------------------------- OMNIDIRECTIONAL SHADOWS PASS -----------------------------------

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Omni Shadows Pass");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 shadowProj;
    float pointNear = 0.1f;
    std::vector<glm::mat4> shadowTransforms;
    std::vector<Entity> pointLightShadowEntities;
    pointLightShadowEntities.reserve(10);

    // get light position, radius, and increase shadowcaster counter
    for (const auto& entity : mEntities)
    {
        if (ecs.HasComponent<CLight>(entity))
        {
            auto& light = ecs.GetComponent<CLight>(entity);

            if (light.shadowCaster && light.type == POINT) 
            {
                pointLightShadowEntities.push_back(entity);
            }
        }
    }
  
    glViewport(0, 0, ((float)1024), ((float)1024));
    glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::mPointLightShadowMapFBO);

    glClear(GL_DEPTH_BUFFER_BIT);

    Shader& pointShadowMapShader = ShaderManager::GetShaderProgram("pointShadowMapShader");

    pointShadowMapShader.use();

    pointShadowMapShader.setInt("omniShadowCasters", pointLightShadowEntities.size());

    int lightEntityCount = 0;

    for (Entity lightEntity : pointLightShadowEntities)
    {

        lightEntityCount++;

        auto& light = ecs.GetComponent<CLight>(lightEntity);
        auto& transform = ecs.GetComponent<CTransform>(lightEntity);

        glm::vec3 lightPos = transform.position + light.offset;

        shadowProj = glm::perspective(glm::radians(90.0f), ((float)1024) / ((float)1024), pointNear, light.radius);

        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

        for (unsigned int j = (lightEntityCount - 1) * 6; j < 6 * lightEntityCount; ++j) {

            pointShadowMapShader.setMat4("shadowMatrices[" + std::to_string(j) + "]", shadowTransforms[j]);
        }

        pointShadowMapShader.setFloat("far_plane", light.radius);
        pointShadowMapShader.setVec3("lightPos", lightPos);

        for (const auto& entity : mEntities)
        {
            if (ecs.HasComponent<CLight>(entity) || (!ecs.HasComponent<CModel>(entity) && !ecs.HasComponent<CMesh>(entity)))
            {
                continue;
            }

            auto& transform = ecs.GetComponent<CTransform>(entity);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);

            glm::mat4 rotMatrix = glm::mat4_cast(transform.rotation);

            model *= rotMatrix;

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

    glPopDebugGroup();

    // ------------------------- LIGHTING PASS -----------------------------------

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Lighting Pass");

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(
        glm::radians(45.0f), (float)Window::screenWidth / (float)Window::screenHeight, 0.1f, 100.0f);
    
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

    // voxelize scene before rendering it
    if (Common::vxgi)
    {
        voxelizeScene(ecs.GetComponent<CTransform>(playerEntity).position, dirLightSpaceMatrix);
    }

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
    unsigned int attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
    glDrawBuffers(6, attachments);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (!Common::showVoxelDebug)
    {
        if (UI::isOpen)
        {
            Raycast::calculateMouseRaycast(projection * player.viewMatrix);
        }

        Shader& vxgiTestShader = ShaderManager::GetShaderProgram("vxgiTestShader");
        vxgiTestShader.use();

        if (pointLightShadowEntities.size() != 0)
        {
            glActiveTexture(GL_TEXTURE10);
            glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, RenderingUtil::mPointLightShadowMap);
            vxgiTestShader.setInt("pointShadowMap", 10);
        }

        vxgiTestShader.setFloat("voxelSize", (2 * Common::voxelizationAreaSize) / float(Common::voxelGridDimensions));

        vxgiTestShader.setBool("vxgi", Common::vxgi);
        vxgiTestShader.setBool("showDiffuseAccumulation", Common::showDiffuseAccumulation);
        vxgiTestShader.setBool("showTotalIndirectDiffuseLight", Common::showTotalIndirectDiffuseLight);
        vxgiTestShader.setFloat("diffuseConeSpread", Common::diffuseConeSpread);
        vxgiTestShader.setFloat("voxelizationAreaSize", Common::voxelizationAreaSize);
        vxgiTestShader.setFloat("vxSpecularBias", Common::vxSpecularBias);
        vxgiTestShader.setFloat("ssrSpecularBias", Common::ssrSpecularBias);
        vxgiTestShader.setFloat("ssrMaxBlurDistance", Common::ssrMaxBlurDistance);
        vxgiTestShader.setFloat("specularStepSizeMultiplier", Common::specularStepSizeMultiplier);
        vxgiTestShader.setFloat("specularConeOriginOffset", Common::specularConeOriginOffset);
        vxgiTestShader.setFloat("showTotalIndirectSpecularLight", Common::showTotalIndirectSpecularLight);
        vxgiTestShader.setFloat("specularConeMaxDistance", Common::specularConeMaxDistance);
        vxgiTestShader.setMat4("view", player.viewMatrix);
        vxgiTestShader.setMat4("projection", projection);
        vxgiTestShader.setVec3("camPos", ecs.GetComponent<CTransform>(playerEntity).position);
        vxgiTestShader.setBool("showNormals", Common::normalsDebug);
        vxgiTestShader.setBool("worldPosDebug", Common::worldPosDebug);
        vxgiTestShader.setBool("albedoDebug", Common::albedoDebug);
        vxgiTestShader.setBool("roughnessDebug", Common::roughnessDebug);
        vxgiTestShader.setBool("metallicDebug", Common::metallicDebug);
        vxgiTestShader.setBool("aoDebug", Common::aoDebug);
        vxgiTestShader.setBool("emissionDebug", Common::emissionDebug);
        vxgiTestShader.setBool("bloom", Common::bloomOn);
        vxgiTestShader.setInt("dirShadowMap", 9);
        vxgiTestShader.setMat4("dirLightSpaceMatrix", dirLightSpaceMatrix);
        vxgiTestShader.setBool("ssaoOn", Common::ssaoOn);
        vxgiTestShader.setBool("showAO", Common::showAO);

        glm::mat3 viewNormalMatrix = glm::transpose(glm::inverse(player.viewMatrix));
        vxgiTestShader.setMat3("viewNormalMatrix", viewNormalMatrix);

        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mDirLightShadowMap);
        glActiveTexture(GL_TEXTURE15);
        glBindTexture(GL_TEXTURE_3D, RenderingUtil::mVoxelTexture);
        vxgiTestShader.setInt("voxelTexture", 15);

        glActiveTexture(GL_TEXTURE16);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mViewPos);
        vxgiTestShader.setInt("gViewPos", 16);

        glActiveTexture(GL_TEXTURE17);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mViewNormalTexture);
        vxgiTestShader.setInt("gViewNormals", 17);

        glActiveTexture(GL_TEXTURE18);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mSSRTexture);
        vxgiTestShader.setInt("ssrTexture", 18);

        glActiveTexture(GL_TEXTURE19);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mSSRBlurredTexture);
        vxgiTestShader.setInt("ssrTextureBlur", 19);


        glActiveTexture(GL_TEXTURE20);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mSSAOBlurTexture);
        vxgiTestShader.setInt("SSAO", 20);

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

            glm::mat4 rotMatrix = glm::mat4_cast(transform.rotation);

            model *= rotMatrix;

            model = glm::scale(model, transform.scale);

            glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
           
            vxgiTestShader.use();

            vxgiTestShader.setMat4("model", model);
            vxgiTestShader.setMat3("normalMatrix", normalMatrix);
            
            if (ecs.HasComponent<CModel>(entity))
            {
                auto& model3d = ecs.GetComponent<CModel>(entity);
                vxgiTestShader.setVec2("textureScaling", glm::vec2(1.0f));
                vxgiTestShader.setBool("hasAOTexture", model3d.hasAOTexture);
                model3d.model.Draw(vxgiTestShader);
            }
            else
            {
                //glDisable(GL_CULL_FACE);
                auto& meshComponent = ecs.GetComponent<CMesh>(entity);
                vxgiTestShader.setVec2("textureScaling", meshComponent.textureScaling);
                meshComponent.mesh.Draw(vxgiTestShader);
                //glEnable(GL_CULL_FACE);
            }
        }
    }
    
    glPopDebugGroup();
    
    if (Common::showVoxelDebug)
    {
        // voxel visualization

        glViewport(0, 0, Window::screenWidth, Window::screenHeight);

        glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::mMsFBO);

        // set both color buffer attachments as the draw buffers before clearing them, otherwise only the first color attachment will get cleared.
        // This would mean that the color buffer which we render the bloom brightness texture onto would not get cleared and the bloom would effect would
        // just accumulate over frames.

        glDrawBuffers(1, attachments);

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        Shader& voxelVisualizationShader = ShaderManager::GetShaderProgram("voxelVisualizationShader");

        voxelVisualizationShader.use();

        glUniform3i(glGetUniformLocation(voxelVisualizationShader.ID, "gridDimensions"), Common::voxelGridDimensions, Common::voxelGridDimensions, Common::voxelGridDimensions);

        voxelVisualizationShader.setMat4("view", player.viewMatrix);
        voxelVisualizationShader.setMat4("projection", projection);
        voxelVisualizationShader.setVec3("camPos", ecs.GetComponent<CTransform>(playerEntity).position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, RenderingUtil::mVoxelTexture);
        voxelVisualizationShader.setInt("voxelTexture", 0);

        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glDrawArrays(GL_POINTS, 0, Common::voxelGridDimensions * Common::voxelGridDimensions * Common::voxelGridDimensions);
    }

   if (Common::bulletLinesDebug)
   {
        Shader& lineDebugShader = ShaderManager::GetShaderProgram("lineDebugShader");

        lineDebugShader.use();

        lineDebugShader.setMat4("projection", projection);
        lineDebugShader.setMat4("view", player.viewMatrix);
       
        BulletDebugDrawer::drawLines();
   }

    // ---------------------------- SKYBOX PASS ---------------------------------------

    unsigned int attachmentsSkybox[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT5};

    glDrawBuffers(2, attachmentsSkybox);
    
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

    // viewPosTexture

    glReadBuffer(GL_COLOR_ATTACHMENT2);  // Read from the second color attachment

    glDrawBuffer(GL_COLOR_ATTACHMENT2);  // Draw to the second color attachment

    glBlitFramebuffer(0, 0, Window::screenWidth, Window::screenHeight, 0, 0, Window::screenWidth, Window::screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // normalTexture

    glReadBuffer(GL_COLOR_ATTACHMENT3);  // Read from the second color attachment

    glDrawBuffer(GL_COLOR_ATTACHMENT3);  // Draw to the second color attachment

    glBlitFramebuffer(0, 0, Window::screenWidth, Window::screenHeight, 0, 0, Window::screenWidth, Window::screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // roughnessTexture

    glReadBuffer(GL_COLOR_ATTACHMENT4);  // Read from the second color attachment

    glDrawBuffer(GL_COLOR_ATTACHMENT4);  // Draw to the second color attachment

    glBlitFramebuffer(0, 0, Window::screenWidth, Window::screenHeight, 0, 0, Window::screenWidth, Window::screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // diffuseColorTexture

    glReadBuffer(GL_COLOR_ATTACHMENT5);  // Read from the second color attachment

    glDrawBuffer(GL_COLOR_ATTACHMENT5);  // Draw to the second color attachment

    glBlitFramebuffer(0, 0, Window::screenWidth, Window::screenHeight, 0, 0, Window::screenWidth, Window::screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

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
    
    // ------------------------------ SSR PASS -----------------------------------------------------------

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "SSR Pass");

    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::mSSRFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Shader& ssrShader = ShaderManager::GetShaderProgram("ssrShader");
    ssrShader.use();
    //ssrShader.setMat4("invView", glm::inverse(player.viewMatrix));

    ssrShader.setMat4("projection", projection);
    ssrShader.setFloat("maxDistance", Common::ssrMaxDistance);
    ssrShader.setFloat("resolution", Common::ssrResolution);
    ssrShader.setInt("steps", Common::ssrSteps);
    ssrShader.setFloat("thickness", Common::ssrThickness);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RenderingUtil::mViewPos);
    ssrShader.setInt("gViewPosition", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, RenderingUtil::mViewNormalTexture);
    ssrShader.setInt("gViewNormal", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, RenderingUtil::mRoughnessTexture);
    ssrShader.setInt("gRoughness", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, RenderingUtil::mDiffuseColorTexture);
    ssrShader.setInt("gFinalImage", 3);

    glBindVertexArray(RenderingUtil::mScreenQuadVAO);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glPopDebugGroup();

    // blur the ssr texture

    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "SSR Blur Pass");

    glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::mBoxBlurFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Shader& boxBlurShader = ShaderManager::GetShaderProgram("boxBlurShader");
    boxBlurShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RenderingUtil::mSSRTexture);
    boxBlurShader.setInt("colorTexture", 0);

    glBindVertexArray(RenderingUtil::mScreenQuadVAO);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glPopDebugGroup();

    //  ------------------------------ SSAO PASS ---------------------------------------------------------------------

    if (Common::ssaoOn)
    {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "SSAO Pass");

        glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::mSSAOFBO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Shader& ssaoShader = ShaderManager::GetShaderProgram("ssaoShader");
        ssaoShader.use();

        ssaoShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mViewPos);
        ssaoShader.setInt("gViewPosition", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mViewNormalTexture);
        ssaoShader.setInt("gViewNormal", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mSSAONoiseTexture);
        ssaoShader.setInt("texNoise", 2);

        glUniform3fv(glGetUniformLocation(ssaoShader.ID, "samples"), RenderingUtil::mSSAOKernel.size(), glm::value_ptr(RenderingUtil::mSSAOKernel[0]));

        ssaoShader.setInt("screenWidth", Common::SCR_WIDTH);
        ssaoShader.setInt("screenHeight", Common::SCR_HEIGHT);
        ssaoShader.setInt("kernelSize", Common::ssaoKernelSize);
        ssaoShader.setFloat("radius", Common::ssaoRadius);
        ssaoShader.setFloat("power", Common::ssaoPower);

        glBindVertexArray(RenderingUtil::mScreenQuadVAO);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glPopDebugGroup();

        // blur the ssao texture

        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "SSAO Blur Pass");

        glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::mSSAOBlurFBO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Shader& ssaoBlurShader = ShaderManager::GetShaderProgram("ssaoBlurShader");
        ssaoBlurShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mSSAOTexture);
        ssaoBlurShader.setInt("ssaoTexture", 0);

        glBindVertexArray(RenderingUtil::mScreenQuadVAO);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glPopDebugGroup();
    }

    // ------------------------------ POST PROCESSING PASS -----------------------------------------------------------

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


void RenderSystem::voxelizeScene(glm::vec3 camPos, glm::mat4 dirLightSpaceMatrix)
{
    if (true)
    {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Voxelization");

        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glClearTexImage(RenderingUtil::mVoxelTexture, 0, GL_RGBA, GL_HALF_FLOAT, clearColor);

        Shader& voxelizationShader = ShaderManager::GetShaderProgram("voxelizationShader");

        voxelizationShader.use();

        voxelizationShader.setFloat("voxelizationAreaSize", Common::voxelizationAreaSize);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, RenderingUtil::mPointLightShadowMap);
        voxelizationShader.setInt("pointShadowMap", 5);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mDirLightShadowMap);
        voxelizationShader.setInt("dirShadowMap", 6);

        voxelizationShader.setMat4("dirLightSpaceMatrix", dirLightSpaceMatrix);

        glViewport(0, 0, Common::voxelGridDimensions, Common::voxelGridDimensions);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        glBindImageTexture(0, RenderingUtil::mVoxelTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        
        for (const auto& entity : mEntities)
        {
            auto& transform = ecs.GetComponent<CTransform>(entity);

            if (!ecs.HasComponent<CModel>(entity) && !ecs.HasComponent<CMesh>(entity))
            {
                continue;
            }

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);

            glm::mat4 rotMatrix = glm::mat4_cast(transform.rotation);

            model *= rotMatrix;

            model = glm::scale(model, transform.scale);

            glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));

            voxelizationShader.setMat4("model", model);
            voxelizationShader.setMat3("normalMatrix", normalMatrix);
            voxelizationShader.setVec3("camPos", camPos);

            if (ecs.HasComponent<CModel>(entity))
            {
                auto& model3d = ecs.GetComponent<CModel>(entity);
                voxelizationShader.setVec2("textureScaling", glm::vec2(1.0f));
                voxelizationShader.setBool("hasAOTexture", model3d.hasAOTexture);
                model3d.model.Draw(voxelizationShader);
            }
            else
            {
                auto& meshComponent = ecs.GetComponent<CMesh>(entity);
                voxelizationShader.setVec2("textureScaling", meshComponent.textureScaling);
                meshComponent.mesh.Draw(voxelizationShader);
            }
        }

        glBindTexture(GL_TEXTURE_3D, RenderingUtil::mVoxelTexture);
        glGenerateMipmap(GL_TEXTURE_3D);

        Common::voxelize = false;

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        
        glPopDebugGroup();
    }
}