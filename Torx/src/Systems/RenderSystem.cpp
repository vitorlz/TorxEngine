#include "RenderSystem.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "../Util/Window.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CTransform.h"
#include "../Components/CMesh.h"
#include "../Util/ShaderManager.h"
#include "../Util/TextureLoader.h"
#include "../AssetLoading/AssetManager.h"


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

    mCubeVAO = AssetManager::CreateCubeVao();
}

void RenderSystem::Update(float deltaTime, Camera& camera)
{
    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
    glEnable(GL_DEPTH_TEST);

    // ------------------------- First Pass -----------------------------------

    Shader ourShader = ShaderManager::GetShaderProgram("ourShader");

    ourShader.use();

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(
        glm::radians(45.0f), (float)Window::screenWidth / (float)Window::screenHeight, 0.1f, 100.0f);

    glEnable(GL_CULL_FACE);

    for (const auto& entity : mEntities) 
    {
        auto const& transform = ecs.GetComponent<CTransform>(entity);
        auto const& mesh = ecs.GetComponent<CMesh>(entity);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        model = glm::scale(model, transform.scale);
        model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0, 0.0));
        model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0, 0.0));
        model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0, 1.0));
        
        ourShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        ourShader.setMat4("view", view);
        ourShader.setMat4("model", model);

        for (Mesh mesh : mesh.meshes) {
            mesh.Draw(ourShader);
        }
    }

    // ---------------------------- SKYBOX PASS ---------------------------------------

    glDisable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);

    Shader skyboxShader = ShaderManager::GetShaderProgram("cubemapShader");

    skyboxShader.use();

    view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

    skyboxShader.setMat4("projection", projection);
    skyboxShader.setMat4("view", view);
    glBindVertexArray(mCubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemapID);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDepthFunc(GL_LESS);
}