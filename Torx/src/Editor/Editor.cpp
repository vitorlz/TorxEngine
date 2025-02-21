#include "Editor.h"
#include "../Core/Common.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CTransform.h"
#include "../Components/CRigidBody.h"
#include "../Components/CSingleton_Input.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Util/Util.h"
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/string_cast.hpp>
#include "EditorCamera.h"
#include "../Scene/Scene.h"
#include "../UI/UI.h"
#include "../Engine.h"

extern Coordinator ecs;

static CSingleton_Input& inputSing = CSingleton_Input::getInstance();

Editor& Editor::getInstance()
{
    static Editor instance;

    return instance;
}

void Editor::RenderGizmo(int selectedEntity)
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
        std::cout << "Can't render gizmo: entity has no transform" << "\n";
        return;
    }

	ImGuizmo::SetOrthographic(false);

    ImGuizmo::SetRect(UI::gameWindowPos.x, UI::gameWindowPos.y , UI::gameWindowSize.x, UI::gameWindowSize.y);

	auto& transform = ecs.GetComponent<CTransform>(selectedEntity);

    // you need to create a camera component and give it to the player and store the projection and view matrix inside it.
    // the way we are getting the view and projection matrix right now is just ugly.


    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
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

                transform.position = translation;
            }
            else
            {
                transform.position = translation;
            }
        }
        else if (currentGizmoOperation == ImGuizmo::ROTATE)
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
        else if (currentGizmoOperation == ImGuizmo::SCALE)
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

void Editor::Update(float dt)
{
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
