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
#include "../Util/ShaderManager.h"
#include "../Util/TextureLoader.h"
#include "../Util/Util.h"
#include "../Rendering/RenderingUtil.h"


extern Coordinator ecs;
void renderSphere();

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
    mBloomBrightnessTexture = RenderingUtil::GetBloomBrightnessTexture();
    RenderingUtil::CreatePingPongFBOs();
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
void RenderSystem::Update(float deltaTime)
{
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
        glBindFramebuffer(GL_FRAMEBUFFER, mPointLightShadowMapFBO);

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

                if (ecs.HasComponent<CLight>(entity))
                {
                    continue;
                }
                auto& transform = ecs.GetComponent<CTransform>(entity);
                auto& model3d = ecs.GetComponent<CModel>(entity);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, transform.position);

                model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0, 0.0));
                model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0, 0.0));
                model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0, 1.0));
                model = glm::scale(model, transform.scale);

                pointShadowMapShader.setMat4("model", model);

                model3d.model.Draw(pointShadowMapShader);
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

    // for bloom
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

  
    Shader& lightingShader = ShaderManager::GetShaderProgram("lightingShader");

    lightingShader.use();

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

    for (const auto& entity : mEntities) 
    {
        auto& transform = ecs.GetComponent<CTransform>(entity);
        auto& model3d = ecs.GetComponent<CModel>(entity);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
     
        model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0, 0.0));
        model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0, 1.0));
        model = glm::scale(model, transform.scale);

        glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));

        /*if (ecs.HasComponent<CLight>(entity)) 
        {
            auto& light = ecs.GetComponent<CLight>(entity);

            Shader& solidColorShader = ShaderManager::GetShaderProgram("solidColorShader");

            solidColorShader.use();
           
            solidColorShader.setMat4("projection", projection);
            solidColorShader.setMat4("view", player.viewMatrix);
            solidColorShader.setMat4("model", model);
            solidColorShader.setVec3("color", ecs.GetComponent<CLight>(entity).diffuse * 1.5f);

            model3d.model.Draw(solidColorShader);

            continue;
        }*/

        lightingShader.use();
        
        if (omniShadowCasters != 0) 
        {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, mPointLightShadowMap);

            lightingShader.setInt("pointShadowMap", 3);

            glUniform1fv(glGetUniformLocation(lightingShader.ID, "point_far_plane"), omniShadowCasters, pointFar.data());
        }
        
        lightingShader.setMat4("projection", projection); 
        lightingShader.setMat4("view", player.viewMatrix);
        lightingShader.setMat4("model", model);
        lightingShader.setMat3("normalMatrix", normalMatrix);
        lightingShader.setBool("showNormals", Common::normalsDebug);
        lightingShader.setBool("worldPosDebug", Common::worldPosDebug);
        lightingShader.setBool("bloom", Common::bloomOn);

        lightingShader.setVec3("cameraPos", ecs.GetComponent<CTransform>(playerEntity).position);
        lightingShader.setVec3("cameraFront", player.front);

        model3d.model.Draw(lightingShader);
    }

    // ----------------------------- PBR TESTING -----------------------------------------------------------------------------------------

    if (Common::pbrDemonstration)
    {
        glDrawBuffers(1, attachments);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        Shader& pbrLightingShader = ShaderManager::GetShaderProgram("pbrLightingShader");
        pbrLightingShader.use();
        glm::mat4 view = player.viewMatrix;
        pbrLightingShader.setMat4("view", view);
        pbrLightingShader.setVec3("camPos", ecs.GetComponent<CTransform>(playerEntity).position);
        pbrLightingShader.setVec3("albedo", 0.5f, 0.0f, 0.0f);
        pbrLightingShader.setFloat("ao", 1.0f);

        pbrLightingShader.setMat4("projection", projection);

        // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
        glm::mat4 model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; ++row)
        {
            pbrLightingShader.setFloat("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                pbrLightingShader.setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (col - (nrColumns / 2)) * spacing,
                    (row - (nrRows / 2)) * spacing,
                    0.0f
                ));
                model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.15f));
                pbrLightingShader.setMat4("model", model);
                pbrLightingShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
                renderSphere();
            }
        }

        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        // keeps the codeprint small.
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            pbrLightingShader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            pbrLightingShader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            pbrLightingShader.setMat4("model", model);
            pbrLightingShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
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
    glBindVertexArray(mCubeVAO);
    skyBoxShader.setInt("skybox", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemapID);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthFunc(GL_LESS);

    // ---------------------------------- BLITTING ---------------------------------------

    // Set up to blit from the first color buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mMsFBO);
    glReadBuffer(GL_COLOR_ATTACHMENT0);  // Read from the first color attachment

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBlittingFBO);
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
                GL_TEXTURE_2D, first_iteration ? mBloomBrightnessTexture : RenderingUtil::mPingPongBuffers[!horizontal]
            );

            glBindVertexArray(mScreenQuadVAO);
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
    postProcessingShader.setFloat("exposure", Common::exposure);
    postProcessingShader.setBool("reinhard", Common::reinhard);
    postProcessingShader.setBool("uncharted2", Common::uncharted);
    postProcessingShader.setBool("ACES", Common::aces);
    postProcessingShader.setBool("bloom", Common::bloomOn);
   
    glBindVertexArray(mScreenQuadVAO);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mScreenQuadTexture);

    if (Common::bloomOn) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, RenderingUtil::mPingPongBuffers[(Common::bloomKernelSize * 2) % 2 ? !horizontal : horizontal]);
    }

    postProcessingShader.setInt("screenQuadTexture", 0);
    postProcessingShader.setInt("bloomBlurTexture", 1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}




// PBR testing
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}
