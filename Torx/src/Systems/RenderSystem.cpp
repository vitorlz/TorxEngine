#include "RenderSystem.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "../Util/Window.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CTransform.h"
#include "../Components/CMesh.h"
#include "../Util/ShaderManager.h"


extern Coordinator ecs;

void RenderSystem::Init() 
{
    
    // ----- Set Main Camera ------------
    
    // ----- Set Shaders ----------------
    //Shader ourShader("res/shaders/testShader.vert", "res/shaders/testShader.frag");
    //mShaders["ourShader"] = ourShader;
}

void RenderSystem::Update(float deltaTime, Camera& camera)
{
    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_DEPTH_TEST);

    // ------------------------- First Pass -----------------------------------

    ShaderManager::GetShaderProgram("ourShader").use();

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
        
        
        glm::mat4 view = camera.GetViewMatrix(); // make sure to initialize matrix to identity matrix first
       
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(
            glm::radians(45.0f), (float)Window::screenWidth / (float)Window::screenHeight, 0.1f, 100.0f);
        // pass transformation matrices to the shader
        ShaderManager::GetShaderProgram("ourShader").setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        ShaderManager::GetShaderProgram("ourShader").setMat4("view", view);
        ShaderManager::GetShaderProgram("ourShader").setMat4("model", model);

        for (Mesh mesh : mesh.meshes) {
            mesh.Draw(ShaderManager::GetShaderProgram("ourShader"));
        }

    }
}