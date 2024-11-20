#include "UI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "../Rendering/RenderingUtil.h"
#include "../Core/Common.h"
#include "../Core/Coordinator.hpp"
#include "../AssetLoading/AssetManager.h"
#include "../Components/CLight.h"
#include "../Components/CModel.h"
#include "../Components/CTransform.h"
#include "../Components/CSingleton_Input.h"
#include "../Components/CPlayer.h"
#include "../Components/CRigidBody.h"
#include "../Components/CMesh.h"
#include "../Physics/Raycast.h"
#include "../Editor/Editor.h"
#include <iomanip>
#include <sstream>

bool UI::isOpen{ false };
bool UI::firstMouseUpdateAfterMenu{ false };

void showComponents(Entity entity);
void showEntityOptions(Entity entity);

extern Coordinator ecs;

void UI::Init(GLFWwindow* window) 
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void UI::NewFrame() 
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void UI::Update() 
{
    ImGui::Begin("Menu");
    ImGui::Shortcut(ImGuiKey_Tab, ImGuiInputFlags_None);
    
    CSingleton_Input& inputSing = CSingleton_Input::getInstance();

    if (ImGui::TreeNode("Stats"))
    { 
        std::stringstream mouseXstream;
        mouseXstream << std::fixed << std::setprecision(2) << inputSing.mouseX;
        std::string mouseX = "Mouse X: " + mouseXstream.str();

        std::stringstream mouseYstream;
        mouseYstream << std::fixed << std::setprecision(2) << inputSing.mouseY;
        std::string mouseY = "Mouse Y: " + mouseYstream.str();

        ImGui::Text(mouseX.c_str());
        ImGui::Text(mouseY.c_str());

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Entities"))
    {
        std::vector<Entity> livingEntities = ecs.GetLivingEntities();

        for (int i = 0; i < livingEntities.size(); i++)
        {
            // Here we use PushID() to generate a unique base ID, and then the "" used as TreeNode id won't conflict.
            // An alternative to using 'PushID() + TreeNode("", ...)' to generate a unique ID is to use 'TreeNode((void*)(intptr_t)i, ...)',
            // aka generate a dummy pointer-sized value to be hashed. The demo below uses that technique. Both are fine.
            ImGui::PushID(i);
            if (ImGui::TreeNode("", "Entity %d", livingEntities[i]))
            {  
                showComponents(livingEntities[i]);
                showEntityOptions(livingEntities[i]);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Tone Mapping"))
    {
        const char* items[] = { "ACES Filmic", "Reinhard Simple", "Uncharted 2"};
        static int item_selected_idx = 0; 

        const char* combo_preview_value = items[item_selected_idx];

        if (ImGui::BeginCombo("combo 1", combo_preview_value))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                ImGui::PushID(n);
                const bool is_selected = (item_selected_idx == n);
                if (ImGui::Selectable(items[n], is_selected)) 
                {
                    item_selected_idx = n;
                   
                    Common::aces = ("ACES Filmic" == items[n]);
                    Common::reinhard = ("Reinhard" == items[n]);
                    Common::uncharted = ("Uncharted 2" == items[n]);
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();

                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

        ImGui::SliderFloat("Exposure", &Common::exposure, 0.0f, 20.0f, "%.5f");
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Bloom"))
    {
        ImGui::Checkbox("Enable bloom", &Common::bloomOn);
        if (Common::bloomOn) {
            ImGui::SliderInt("Kernel Size", &Common::bloomKernelSize, 0, 20);
            ImGui::SliderFloat("Std deviation", &Common::bloomStdDeviation, 0.001f, 10.0f);
            ImGui::SliderFloat("Interval Multiplier", &Common::bloomIntervalMultiplier, 0.0f, 1.0f);

            ImGui::PlotHistogram(
                "Weights", Common::bloomWeights.data(), Common::bloomWeights.size(), 0, NULL, 0.0f, 0.25f, ImVec2(0, 80.0f));
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Environment Maps"))
    {
        const char* items[] = { "Lilienstein", "Sunset", "Clear Night", "Cinema", "Fireplace"};
        static int item_selected_idx = 0;

        const char* combo_preview_value = items[item_selected_idx];

        if (ImGui::BeginCombo("combo 1", combo_preview_value))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                ImGui::PushID(n);
                const bool is_selected = (item_selected_idx == n);
                if (ImGui::Selectable(items[n], is_selected))
                {
                    item_selected_idx = n;

                    if ("Lilienstein" == items[n])
                    {

                        glDeleteTextures(1, &RenderingUtil::mEnvironmentCubemap);
                        glDeleteTextures(1, &RenderingUtil::mIrradianceCubemap);
                        glDeleteTextures(1, &RenderingUtil::mPrefilteredEnvMap);
                        glDeleteTextures(1, &RenderingUtil::mBrdfLUT);

                        RenderingUtil::EquirectangularToCubemap("res/textures/hdr/lilienstein_2k.hdr");
                        RenderingUtil::CreateIrradianceCubemap();
                        RenderingUtil::CreatePrefilteredEnvMap();
                        RenderingUtil::CreateBRDFIntegrationMap();
                    }

                    if ("Sunset" == items[n])
                    {
                        glDeleteTextures(1, &RenderingUtil::mEnvironmentCubemap);
                        glDeleteTextures(1, &RenderingUtil::mIrradianceCubemap);
                        glDeleteTextures(1, &RenderingUtil::mPrefilteredEnvMap);
                        glDeleteTextures(1, &RenderingUtil::mBrdfLUT);

                        RenderingUtil::EquirectangularToCubemap("res/textures/hdr/sunset_jhbcentral_2k.hdr");
                        RenderingUtil::CreateIrradianceCubemap();
                        RenderingUtil::CreatePrefilteredEnvMap();
                        RenderingUtil::CreateBRDFIntegrationMap();
                    }

                    if ("Clear Night" == items[n])
                    {
                        glDeleteTextures(1, &RenderingUtil::mEnvironmentCubemap);
                        glDeleteTextures(1, &RenderingUtil::mIrradianceCubemap);
                        glDeleteTextures(1, &RenderingUtil::mPrefilteredEnvMap);
                        glDeleteTextures(1, &RenderingUtil::mBrdfLUT);

                        RenderingUtil::EquirectangularToCubemap("res/textures/hdr/rogland_clear_night_2k.hdr");
                        RenderingUtil::CreateIrradianceCubemap();
                        RenderingUtil::CreatePrefilteredEnvMap();
                        RenderingUtil::CreateBRDFIntegrationMap();
                    }

                    if ("Cinema" == items[n])
                    {
                        glDeleteTextures(1, &RenderingUtil::mEnvironmentCubemap);
                        glDeleteTextures(1, &RenderingUtil::mIrradianceCubemap);
                        glDeleteTextures(1, &RenderingUtil::mPrefilteredEnvMap);
                        glDeleteTextures(1, &RenderingUtil::mBrdfLUT);

                        RenderingUtil::EquirectangularToCubemap("res/textures/hdr/pretville_cinema_2k.hdr");
                        RenderingUtil::CreateIrradianceCubemap();
                        RenderingUtil::CreatePrefilteredEnvMap();
                        RenderingUtil::CreateBRDFIntegrationMap();
                    }

                    if ("Fireplace" == items[n])
                    {
                        glDeleteTextures(1, &RenderingUtil::mEnvironmentCubemap);
                        glDeleteTextures(1, &RenderingUtil::mIrradianceCubemap);
                        glDeleteTextures(1, &RenderingUtil::mPrefilteredEnvMap);
                        glDeleteTextures(1, &RenderingUtil::mBrdfLUT);

                        RenderingUtil::EquirectangularToCubemap("res/textures/hdr/fireplace_2k.hdr");
                        RenderingUtil::CreateIrradianceCubemap();
                        RenderingUtil::CreatePrefilteredEnvMap();
                        RenderingUtil::CreateBRDFIntegrationMap();
                    }
                    
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();

                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }


    if (ImGui::TreeNode("Debug"))
    {
        ImGui::Checkbox("Show normals", &Common::normalsDebug);
        ImGui::Checkbox("Show World Position", &Common::worldPosDebug);
        ImGui::Checkbox("Wireframe mode", &Common::wireframeDebug);
        ImGui::Checkbox("PBR demo", &Common::pbrDemonstration);
        ImGui::Checkbox("Show light position", &Common::lightPosDebug);
        ImGui::Checkbox("Show albedo ", &Common::albedoDebug);
        ImGui::Checkbox("Show roughness", &Common::roughnessDebug);
        ImGui::Checkbox("Show metalness", &Common::metallicDebug);
        ImGui::Checkbox("Show physics debug lines", &Common::bulletLinesDebug);

        ImGui::TreePop();
    }

    ImGui::End();

    // Gizmos
    
    ImGui::Begin("Editor");

    if (ImGui::RadioButton("Translate", Editor::GetCurrentGizmoOperation() == ImGuizmo::TRANSLATE))
        Editor::SetCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", Editor::GetCurrentGizmoOperation() == ImGuizmo::ROTATE))
        Editor::SetCurrentGizmoOperation(ImGuizmo::ROTATE);
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", Editor::GetCurrentGizmoOperation() == ImGuizmo::SCALE))
        Editor::SetCurrentGizmoOperation(ImGuizmo::SCALE);

    static int selectedEntity{ -1 };
    static bool addingNewEntity{ false };
    static Entity newEntity;

    if (!ImGuizmo::IsOver() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && !addingNewEntity)
    {
        
        selectedEntity = Raycast::getSelectedEntity();
    }

    if (selectedEntity >= 0 && ecs.isAlive(selectedEntity))
    {
        if (!addingNewEntity)
        {
            ImGui::Separator();

            Editor::RenderGizmo(selectedEntity);

            std::stringstream ss;
            ss << "Selected Entity: " << selectedEntity;
            std::string selectedEntityText = ss.str();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), selectedEntityText.c_str());
            showComponents(selectedEntity);
            showEntityOptions(selectedEntity);
        }
    }

    ImGui::Separator();
 
    if (!addingNewEntity)
    {
        if (ImGui::Button("Add Entity"))
        {
            addingNewEntity = true;
            newEntity = ecs.CreateEntity();
        }
    }

    if (addingNewEntity) 
    {
        std::stringstream ss;
        ss << "Adding Entity: " << newEntity;
        std::string addingEntityText = ss.str();
        
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), addingEntityText.c_str());

        showComponents(newEntity);
        showEntityOptions(newEntity);

        if (ImGui::Button("Done"))
        {
            addingNewEntity = false;
        }
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::Terminate() 
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void showComponents(Entity entity)
{
    if (ecs.HasComponent<CLight>(entity))
    {
        CLight& light = ecs.GetComponent<CLight>(entity);
        if (ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_AllowItemOverlap))
        {
            if (light.type == DIRECTIONAL)
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Type: directional light");
            }
            else if (light.type == POINT)
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Type: point light");
            }
            else if (light.type == SPOT)
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Type: spotlight");

                ImGui::InputFloat3("Direction", &light.direction.x);
                ImGui::SliderFloat("Inner Cutoff", &light.innerCutoff, 0.1f, light.outerCutoff, "angle = %.1f");
                ImGui::SliderFloat("Outer Cutoff", &light.outerCutoff, light.innerCutoff, 90.0f, "angle = %.1f");
            }

            ImGui::ColorEdit3("Color", &light.color.x);
            ImGui::SliderFloat("Radius", &light.radius, 0.001f, 100.0f, "%.5f");
            ImGui::SliderFloat("Strength", &light.strength, 0.001f, 100.0f, "%.2f");
            if (light.type == POINT || light.type == SPOT)
            {
                ImGui::SliderFloat3("Offset", &light.offset.x, -10.0f, 10.0f);
            }
            ImGui::Checkbox("Cast Shadows", (bool*)&light.shadowCaster);

            light.isDirty = true;

        }
        ImGui::SameLine();
        if (ImGui::Button("Delete##xx0"))
        {
            ecs.RemoveComponent<CLight>(entity);
        }
    }

    if (ecs.HasComponent<CTransform>(entity))
    {
        if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_AllowItemOverlap))
        {
            CTransform& transform = ecs.GetComponent<CTransform>(entity);

            ImGui::SliderFloat3("Position", &transform.position.x, -20.0f, 20.0f, "%.3f");
            ImGui::SliderFloat3("Scale", &transform.scale.x, -10.0f, 10.0f, "%.3f");
            ImGui::SliderFloat3("Rotation", &transform.rotation.x, -360.0f, 360.0f, "%.3f");

            if (ecs.HasComponent<CLight>(entity))
            {
                CLight& light = ecs.GetComponent<CLight>(entity);
                light.isDirty = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete##xx1"))
        {
            ecs.RemoveComponent<CTransform>(entity);
        }
    }

    if (ecs.HasComponent<CModel>(entity))
    {
        if (ImGui::CollapsingHeader("Model Component", ImGuiTreeNodeFlags_AllowItemOverlap))
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Has model component");
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete##xx2"))
        {
            ecs.RemoveComponent<CModel>(entity);
        }
    }

    if (ecs.HasComponent<CPlayer>(entity))
    {
        if (ImGui::CollapsingHeader("Player Component", ImGuiTreeNodeFlags_AllowItemOverlap))
        {
            CPlayer& player = ecs.GetComponent<CPlayer>(entity);

            ImGui::InputFloat3("Front", &player.front.x);
            ImGui::InputFloat3("Right", &player.right.x);
            ImGui::InputFloat3("Up", &player.up.x);
            ImGui::SliderFloat("Movement Speed", &player.movementSpeed, 1.0f, 10.0f, "%.2f");
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete##xx3"))
        {
            ecs.RemoveComponent<CPlayer>(entity);
        }
    }

    if (ecs.HasComponent<CRigidBody>(entity))
    {
        if (ImGui::CollapsingHeader("RigidBody Component", ImGuiTreeNodeFlags_AllowItemOverlap))
        {
            auto& rigidBody = ecs.GetComponent<CRigidBody>(entity);

            float& mass = (float&)rigidBody.mass;

            if (rigidBody.mass > 0)
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Dynamic body");
            }
            else 
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Static Body");
            }
            ImGui::SliderFloat("Mass", &mass, 1.0f, 10.0f, "%.2f");

        }
        ImGui::SameLine();
        if (ImGui::Button("Delete##xx4"))
        {
            ecs.RemoveComponent<CRigidBody>(entity);
        }
    }

}

void showEntityOptions(Entity entity)
{

    static int selectedComponent = -1;
    const char* Components[] = { "Transform", "Mesh", "Model", "Static Rigid body", "Dynamic Rigid body", "Light" };

    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("my_select_popup");
    
    if (ImGui::BeginPopup("my_select_popup"))
    {
        ImGui::SeparatorText("Select a component");
        for (int i = 0; i < IM_ARRAYSIZE(Components); i++)
            if (ImGui::Selectable(Components[i]))
                selectedComponent = i;

        ImGui::EndPopup();
    }
    else if (Components[selectedComponent] == "Transform" && !ecs.HasComponent<CTransform>(entity))
    {
        ecs.AddComponent<CTransform>(
            entity,
            CTransform{
                .position = glm::vec3(0.0f, 5.0f, 0.0f),
                .scale = glm::vec3(1.0f),
                .rotation = glm::vec3(0.0f, 0.0f, 0.0f),
            });

        selectedComponent = -1;
    }
    else if (Components[selectedComponent] == "Mesh" && !ecs.HasComponent<CMesh>(entity))
    {
        ecs.AddComponent<CMesh>(
            entity,
            CMesh{
                .mesh = AssetManager::GetMesh("cube"),
            });

        selectedComponent = -1;
    }
    else if (Components[selectedComponent] == "Static Rigid body" && !ecs.HasComponent<CRigidBody>(entity))
    {
        ecs.AddComponent<CRigidBody>(
            entity,
            CRigidBody{
                .mass = 0.f
            });

        selectedComponent = -1;
    }
    else if (Components[selectedComponent] == "Dynamic Rigid body" && !ecs.HasComponent<CRigidBody>(entity))
    {
        ecs.AddComponent<CRigidBody>(
            entity,
            CRigidBody{
                .mass = 1.f
            });

        selectedComponent = -1;
    }
    else if (ImGui::BeginMenu("Sub-menu"))
    {
        ImGui::MenuItem("Click me");
        ImGui::EndMenu();
    }


    if (ImGui::Button("Destroy Entity"))
    {
        ecs.DestroyEntity(entity);
    }
}