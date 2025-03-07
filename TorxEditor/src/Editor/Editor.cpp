#define GLM_ENABLE_EXPERIMENTAL 
#define GLFW_INCLUDE_NONE
#include "Editor.h"
#include "Core/Common.h"
#include "Core/Coordinator.hpp"
#include "Components/CTransform.h"
#include "Components/CRigidBody.h"
#include "Components/CSingleton_Input.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Util/Util.h"

#include <glm/gtx/string_cast.hpp>
#include "EditorCamera.h"
#include "Scene/Scene.h"
#include "../UI/UI.h"
#include "Engine.h"
#include <Misc/TextRendering.h>
#include "Util/RenderingUtil.h"
#include "AssetLoading/AssetManager.h"
#include "Util/ShaderManager.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "Components/CLight.h"
#include "Components/CTransform.h"
#include "Components/CModel.h"
#include "Components/CMesh.h"

extern Coordinator ecs;

static CSingleton_Input& inputSing = CSingleton_Input::getInstance();

Editor& Editor::getInstance()
{
    static Editor instance;

    return instance;
}

void Editor::RenderGizmo(int selectedEntity, bool isLightIcon)
{
    ImGui::Shortcut(ImGuiKey_Tab, ImGuiInputFlags_None);
    ImGui::Separator();

    if (ImGui::RadioButton("Translate", Editor::GetCurrentGizmoOperation() == ImGuizmo::TRANSLATE))
        Editor::SetCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", Editor::GetCurrentGizmoOperation() == ImGuizmo::ROTATE))
        Editor::SetCurrentGizmoOperation(ImGuizmo::ROTATE);
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", Editor::GetCurrentGizmoOperation() == ImGuizmo::SCALE))
        Editor::SetCurrentGizmoOperation(ImGuizmo::SCALE);

    ImGui::Separator();

    ImGui::Checkbox("##xx1", &useSnap);
    ImGui::SameLine();

    if (currentGizmoOperation == ImGuizmo::TRANSLATE)
    {
        ImGui::InputFloat3("Translation Snap", &translateSnap[0]);
        snap = translateSnap;
    }
    else if (currentGizmoOperation == ImGuizmo::ROTATE)
    {
        ImGui::InputFloat("Rotation Snap", &rotateSnap[0]);
        snap = rotateSnap;
    }
    else if (currentGizmoOperation == ImGuizmo::SCALE)
    {
        ImGui::InputFloat("Scale Snap", &scaleSnap[0]);
        snap = scaleSnap;
    }
       
    ImGui::Checkbox("Bound sizing", &boundSizing);
        
    if (boundSizing)
    {
        ImGui::Checkbox("##xx2", &boundSizingSnap);
        ImGui::SameLine();
        ImGui::InputFloat3("Bound Snap", boundsSnap);
    }

    if (!ecs.HasComponent<CTransform>(selectedEntity))
    {
        //std::cout << "Can't render gizmo: entity has no transform" << "\n";
        return;
    }

	ImGuizmo::SetOrthographic(false);

    ImGuizmo::SetRect(UI::gameWindowPos.x, UI::gameWindowPos.y , UI::gameWindowSize.x, UI::gameWindowSize.y);

	auto& transform = ecs.GetComponent<CTransform>(selectedEntity);

    // you need to create a camera component and give it to the player and store the projection and view matrix inside it.
    // the way we are getting the view and projection matrix right now is just ugly.
    CLight* light;

    glm::mat4 model = glm::mat4(1.0f);

    if (isLightIcon && ecs.HasComponent<CLight>(selectedEntity))
    {
        light = &ecs.GetComponent<CLight>(selectedEntity);
        model = glm::translate(model, transform.position + light->offset);
    }
    else
    {
        model = glm::translate(model, transform.position);
    }

    glm::mat4 rotMatrix = glm::mat4_cast(transform.rotation);
    model *= rotMatrix;
    model = glm::scale(model, transform.scale);

    if (inputSing.keyDown[TORX_KEY_G])
        currentGizmoOperation = ImGuizmo::TRANSLATE;
    if (inputSing.keyDown[TORX_KEY_R])
        currentGizmoOperation = ImGuizmo::ROTATE;
    if (inputSing.keyDown[TORX_KEY_S])
        currentGizmoOperation = ImGuizmo::SCALE;

    ImGuizmo::Manipulate(glm::value_ptr(Common::currentViewMatrix), glm::value_ptr(Common::currentProjMatrix), currentGizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(model), NULL, useSnap ? snap.data() : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

    if (ImGuizmo::IsUsing())
    {
        Common::usingGuizmo = true;

        glm::vec3 translation, scale;
        glm::quat rotation;
           
        Util::decomposeMtx(model, translation, rotation, scale);

        if (currentGizmoOperation == ImGuizmo::TRANSLATE)
        {
            if (useSnap)
            {
                translation.x = round(translation.x / snap[0]) * snap[0];
                translation.y = round(translation.y / snap[1]) * snap[1];
                translation.z = round(translation.z / snap[2]) * snap[2];

                if (isLightIcon && (ecs.HasComponent<CModel>(selectedEntity) || ecs.HasComponent<CMesh>(selectedEntity)) && light)
                {
                    light->offset = translation - transform.position;
                }
                else
                {
                    transform.position = translation;
                } 
            }
            else
            {
                if (isLightIcon && (ecs.HasComponent<CModel>(selectedEntity) || ecs.HasComponent<CMesh>(selectedEntity)) && light)
                {
                    light->offset = translation - transform.position;
                }
                else
                {
                    transform.position = translation;
                }
            }
        }
        else if (currentGizmoOperation == ImGuizmo::ROTATE && !isLightIcon)
        {
            if (useSnap)
            {
                glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(rotation));

                eulerAngles = round(eulerAngles / rotateSnap[0]) * rotateSnap[0];
                    
                transform.rotation = glm::quat(glm::radians(eulerAngles));
            }
            else
            {
                transform.rotation = rotation;
            }
               
        }
        else if (currentGizmoOperation == ImGuizmo::SCALE && !isLightIcon)
        {
            if (useSnap)
            {
                scale = round(scale / snap[0]) * snap[0];
                transform.scale = scale;
            }
            else
            {
                transform.scale = scale;
            }
        }
    }
    else
    {
        Common::usingGuizmo = false;
    }
}

void Editor::Run()
{
    Torx::Engine& engine = Torx::Engine::getInstance();
    RenderingUtil::gFinalRenderTarget = RenderingUtil::gGameWindowFBO;

    AssetManager::LoadAssets();

    float deltaTime{};
    float lastFrame{};

    while (!glfwWindowShouldClose(engine.GetWindow().GetPointer()))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ECSCore::UpdateSystems(deltaTime);

        Update(deltaTime);
        engine.GetWindow().Update();
    }

    FT_Done_FreeType(TextRendering::m_ft);
    engine.GetWindow().Terminate();
    GetUI().Terminate();
}
void Editor::InitUI()
{
    ui.Init(Torx::Engine::getInstance().GetWindow().GetPointer());
}

UI& Editor::GetUI()
{
    return ui;
}

int  Editor::GetCurrentGizmoMode()
{
    return currentGizmoMode;
}
int  Editor::GetCurrentGizmoOperation()
{
    return currentGizmoOperation;
}

void  Editor::SetCurrentGizmoMode(int gizmoMode)
{
    currentGizmoMode = (ImGuizmo::MODE)gizmoMode;
}
void  Editor::SetCurrentGizmoOperation(int gizmoOperation)
{
    currentGizmoOperation = (ImGuizmo::OPERATION)gizmoOperation;
}

bool  Editor::isOn()
{
    return editorOn;
}

void  Editor::setStatus(bool status)
{
    editorOn = status;
}

EditorCamera& Editor::GetEditorCamera()
{
    return editorCamera;
}

void Editor::RenderIcons()
{
    if (Torx::Engine::MODE == Torx::EDITOR)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, RenderingUtil::gBufferFBO);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RenderingUtil::gFinalRenderTarget);

        glBlitFramebuffer(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, 0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::gFinalRenderTarget);

        std::vector<Entity> entities = ecs.GetLivingEntities();

        TextRendering fontawesome = AssetManager::GetTextFont("fontawesome");
        Shader& iconShader = ShaderManager::GetShaderProgram("iconShader");

         iconShader.use();

        iconShader.setMat4("projection", Common::currentProjMatrix);
        iconShader.setMat4("view", Common::currentViewMatrix);

        for (const auto& entity : entities)
        {
            if (!ecs.HasComponent<CTransform>(entity))
                continue;

            auto& transform = ecs.GetComponent<CTransform>(entity);

            if (ecs.HasComponent<CLight>(entity))
            {
                auto& light = ecs.GetComponent<CLight>(entity);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glm::vec3 worldPos = transform.position + light.offset;

                glDepthMask(GL_FALSE);

                int unicode;

                glm::vec3 color = light.color;

                if (light.type == POINT)
                {
                    iconShader.setVec3("iconOffset", glm::vec3(0.0f, 0.25f, 0.0f));
                    unicode = 0xf0eb;
                }
                else 
                {
                    color = glm::vec3(1.0f, 1.0f, 0.0f);
                    iconShader.setVec3("iconOffset", glm::vec3(0.0f, 0.4f, -0.12f));
                    unicode = 0xf185;
                }

                fontawesome.RenderIcon(iconShader, unicode,
                    0.0f, 0.0f, 0.5f, worldPos, color);
                
                glDepthMask(GL_TRUE);
                glDisable(GL_BLEND);  
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, RenderingUtil::gBufferFBO);
        glDrawBuffer(GL_COLOR_ATTACHMENT7);

        for (auto entity : entities)
        {
            if (!ecs.HasComponent<CTransform>(entity))
                continue;

            auto& transform = ecs.GetComponent<CTransform>(entity);

            if (ecs.HasComponent<CLight>(entity))
            {
                auto& light = ecs.GetComponent<CLight>(entity);

                int unicode;

                if (light.type == POINT)
                {
                    entity += 1000;
                    unicode = 0xf0eb;
                }
                else
                {
                    unicode = 0xf185;
                }

                int r = (entity & 0x000000FF) >> 0;
                int g = (entity & 0x0000FF00) >> 8;
                int b = (entity & 0x00FF0000) >> 16;

                glm::vec3 meshIdColor(r / 255.0f, g / 255.0f, b / 255.0f);
                
                glm::vec3 worldPos = transform.position + light.offset;

                fontawesome.RenderIcon(iconShader, unicode,
                    0.0f, 0.0f, 0.5f, worldPos, meshIdColor);
            }
        }
    }
}

void Editor::Update(float dt)
{
    if (Common::lightPosDebug)
    {
        RenderIcons();
    }

    ui.NewFrame();
   
    editorCamera.Update(dt);
    ui.Update();

    if (Torx::Engine::MODE == Torx::EDITOR && Window::cursorHidden)
    {
        Window::ShowCursor();
    }
    
    if (inputSing.keyPressed[TORX_KEY_ESCAPE] && Torx::Engine::MODE == Torx::PLAY)
    {
        if (!Window::cursorHidden)
        {
            Window::HideCursor();
        }
        else
        {
            inputSing.firstMouse = true;
            Window::ShowCursor();
        }
    } 
}
