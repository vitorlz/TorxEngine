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
#include "../Scene/Scene.h"
#include <filesystem>
#include <iomanip>
#include <sstream>
#include "../Util/Util.h"

bool UI::isOpen{ false };
bool UI::firstMouseUpdateAfterMenu{ false };

void showComponents(Entity entity);
void showEntityOptions(Entity entity, bool addingNewEntity);

extern Coordinator ecs;

Entity playerEntity;

void UI::Init(GLFWwindow* window) 
{
    for (Entity entity : ecs.GetLivingEntities())
    {
        if (ecs.HasComponent<CPlayer>(entity))
        {
            playerEntity = entity;
        }
    }
   
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
    
    
    static bool editorMode{ false };
    ImGui::Checkbox("Editor Mode", &editorMode);
    Editor::setStatus(editorMode);

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
                showEntityOptions(livingEntities[i], false);
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
   
       
        if (ImGui::BeginCombo("##xx", "Select an environment map"))
        {
            std::vector<std::string> envMaps;
            std::string path = "res/textures/hdr";
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                envMaps.push_back(entry.path().filename().string());
            }

            for (int i = 0; i < envMaps.size(); i++)
            {
                ImGui::PushID(i);
                if (ImGui::Selectable(envMaps[i].c_str()))
                {
                    Scene::SetEnvironmentMap(envMaps[i]);
                    RenderingUtil::LoadNewEnvironmentMap(envMaps[i].c_str());
                }

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
        ImGui::Checkbox("Show ambient occlusion", &Common::aoDebug);
        ImGui::Checkbox("Show emission", &Common::emissionDebug);
        ImGui::Checkbox("Show physics debug lines", &Common::bulletLinesDebug);

        ImGui::TreePop();
    }

    static int voxelGridDimensions = Common::voxelGridDimensions;
    if (ImGui::TreeNode("VXGI"))
    {
        ImGui::Checkbox("VXGI on", &Common::vxgi);
       
        ImGui::Checkbox("Show voxelized scene", &Common::showVoxelDebug);
        ImGui::SliderFloat("Voxelization area size", &Common::voxelizationAreaSize, 0.0f, 50.0f);

        ImGui::SeparatorText("Indirect Diffuse Lighting");
        ImGui::Checkbox("Show indirect diffuse light accumulation", &Common::showDiffuseAccumulation);
        ImGui::Checkbox("Show total indirect diffuse light", &Common::showTotalIndirectDiffuseLight);
        ImGui::SliderFloat("Diffuse cone spread", &Common::diffuseConeSpread, 0.001f, 5.0f);

        ImGui::SeparatorText("Indirect specular lighting");
        ImGui::SliderFloat("Specular bias", &Common::specularBias, 0.0f, 20.0f);
        ImGui::SliderFloat("Cone origin offset", &Common::specularConeOriginOffset, 0.0f, 20.0f);
        ImGui::SliderFloat("Cone max. distance", &Common::specularConeMaxDistance, 0.1f, 20.0f);
        ImGui::SliderFloat("Step size multiplier", &Common::specularStepSizeMultiplier, 0.1f, 5.0f);
        ImGui::Checkbox("Show total indirect specular light", &Common::showTotalIndirectSpecularLight);
        
        ImGui::SeparatorText("Voxel grid dimension");
        if (ImGui::RadioButton("64", &voxelGridDimensions, 64))
        {
            RenderingUtil::DeleteTexture(RenderingUtil::mVoxelTexture);
            Common::voxelGridDimensions = voxelGridDimensions;
            RenderingUtil::CreateVoxelTexture(voxelGridDimensions);
            Common::voxelize = true;
        } 
        ImGui::SameLine();
        if (ImGui::RadioButton("128", &voxelGridDimensions, 128))
        {
            RenderingUtil::DeleteTexture(RenderingUtil::mVoxelTexture);
            Common::voxelGridDimensions = voxelGridDimensions;
            RenderingUtil::CreateVoxelTexture(voxelGridDimensions);
            Common::voxelize = true;
            
        } 
        ImGui::SameLine();
        if (ImGui::RadioButton("256", &voxelGridDimensions, 256))
        {
            RenderingUtil::DeleteTexture(RenderingUtil::mVoxelTexture);
            Common::voxelGridDimensions = voxelGridDimensions;
            RenderingUtil::CreateVoxelTexture(voxelGridDimensions);
            Common::voxelize = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("512", &voxelGridDimensions, 512))
        {
            RenderingUtil::DeleteTexture(RenderingUtil::mVoxelTexture);
            Common::voxelGridDimensions = voxelGridDimensions;
            RenderingUtil::CreateVoxelTexture(voxelGridDimensions);
            Common::voxelize = true;
        }

        if (ImGui::Button("Voxelize"))
        {
            Common::voxelize = true;
        }

        ImGui::TreePop();
    }

    if (ImGui::Button("Save scene"))
    {
        Scene::SaveSceneToJson("sponzascene.json");
    }

    //ImGui::Checkbox("Show voxel debug", &Common::showVoxelDebug);

    ImGui::End();

    // Gizmos
    
    if (Editor::isOn())
    {
        ImGui::Begin("Editor");

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
                Editor::RenderGizmo(selectedEntity);

                std::stringstream ss;
                ss << "Selected Entity: " << selectedEntity;
                std::string selectedEntityText = ss.str();
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), selectedEntityText.c_str());
                showComponents(selectedEntity);
                showEntityOptions(selectedEntity, addingNewEntity);
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
            showEntityOptions(newEntity, addingNewEntity);

            if (ImGui::Button("Done"))
            {
                addingNewEntity = false;
            }
        }

        ImGui::End();
    }

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
        light.isDirty = true;
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

            ImGui::SliderFloat3("Position", &transform.position.x, -20.0f, 20.0f, "%.10f");
            ImGui::SliderFloat3("Scale", &transform.scale.x, -10.0f, 10.0f, "%.10f");

            glm::vec3 rotationEuler = glm::degrees(glm::eulerAngles(transform.rotation));

            ImGui::SliderFloat3("Rotation", &rotationEuler.x, -360.0f, 360.0f, "%.10f");
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
            auto& modelComponent = ecs.GetComponent<CModel>(entity);

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), modelComponent.modelName.c_str());
        }
        ImGui::SameLine();

        static bool dont_ask_me_next_time = false;
        if (ImGui::Button("Delete##xx2"))
        {
            if (ecs.HasComponent<CRigidBody>(entity) && !dont_ask_me_next_time) 
            {
                ImGui::OpenPopup("Delete?");
            }
            else if (ecs.HasComponent<CRigidBody>(entity))
            {
                ecs.RemoveComponent<CModel>(entity);
                ecs.RemoveComponent<CRigidBody>(entity);
            }
            else
            {
                ecs.RemoveComponent<CModel>(entity);
            }
            
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Deleting an entity's model component will also delete its rigid body.");
            ImGui::Separator();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            ImGui::PopStyleVar();

            if (ImGui::Button("OK", ImVec2(120, 0))) 
            { 
                ecs.RemoveComponent<CModel>(entity);
                ecs.RemoveComponent<CRigidBody>(entity);
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
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
                ImGui::SliderFloat("Mass", &mass, 0.01f, 100.0f, "%.2f");
            }
            else 
            {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Static Body");
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Delete##xx4"))
        {
            ecs.RemoveComponent<CRigidBody>(entity);
        }
    }

    if (ecs.HasComponent<CMesh>(entity))
    {
        if (ImGui::CollapsingHeader("Mesh Component", ImGuiTreeNodeFlags_AllowItemOverlap))
        {
            auto& meshComponent = ecs.GetComponent<CMesh>(entity);

            if (ImGui::BeginCombo("", "Choose texture"))
            {
                std::vector<std::string> textureTags;
                std::string path = "res/textures/pbr";
                for (const auto& entry : std::filesystem::directory_iterator(path))
                {
                    textureTags.push_back(entry.path().filename().string());
                }

                for (int i = 0; i < textureTags.size(); i++)
                {
                    if (ImGui::Selectable(textureTags[i].c_str()))
                    {     
                        meshComponent.texture = textureTags[i];
                        meshComponent.mesh.textures =  AssetManager::LoadMeshTextures(meshComponent.texture.c_str());
                    }

                }
                ImGui::EndCombo();
            }

            ImGui::SliderFloat2("Texture scaling factor", &meshComponent.textureScaling.x, 0.1f, 100.0f, "%.3f");
        }

        ImGui::SameLine();
        static bool dont_ask_me_next_time = false;
        if (ImGui::Button("Delete##xx2"))
        {

            if (ecs.HasComponent<CRigidBody>(entity))
            {
                ImGui::OpenPopup("Delete?");
            }
            else if (ecs.HasComponent<CRigidBody>(entity))
            {
                ecs.RemoveComponent<CMesh>(entity);
                ecs.RemoveComponent<CRigidBody>(entity);
            }
            else
            {
                ecs.RemoveComponent<CMesh>(entity);
            }

        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Deleting an entity's mesh component will also delete its rigid body.");
            ImGui::Separator();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            ImGui::PopStyleVar();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                ecs.RemoveComponent<CMesh>(entity);
                ecs.RemoveComponent<CRigidBody>(entity);
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
    }

}

void showEntityOptions(Entity entity, bool addingNewEntity)
{

    static int selectedComponent = -1;
    const char* singleChoiceComponents[] = { "Transform" };
    const char* multipleChoiceComponents[] = { "Mesh", "Model", "Rigid body", "Light"};

    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("my_select_popup");
    
    if (ImGui::BeginPopup("my_select_popup"))
    {
        ImGui::SeparatorText("Select a component");
        for (int i = 0; i < IM_ARRAYSIZE(singleChoiceComponents); i++)
        {
            if (ImGui::Selectable(singleChoiceComponents[i]))
            {
                selectedComponent = i;
            }       
        }

        for (int i = 0; i < IM_ARRAYSIZE(multipleChoiceComponents); i++)
        {
            if (ImGui::BeginMenu(multipleChoiceComponents[i]))
            {
                if (multipleChoiceComponents[i] == "Mesh" && !ecs.HasComponent<CMesh>(entity))
                {
                    if (ImGui::MenuItem("Cube"))
                    {
                        ecs.AddComponent<CMesh>(
                            entity,
                            CMesh{
                                .mesh = AssetManager::GetMesh("cube"),
                                .meshType = "cube"
                            });
                    }

                    if (ImGui::MenuItem("Quad"))
                    {
                        ecs.AddComponent<CMesh>(
                            entity,
                            CMesh{
                                .mesh = AssetManager::GetMesh("quad"),
                                .meshType = "quad"
                            });
                    }
                }
                if (multipleChoiceComponents[i] == "Rigid body" && !ecs.HasComponent<CRigidBody>(entity))
                {
                    if (ImGui::MenuItem("Static"))
                    {
                        ecs.AddComponent<CRigidBody>(
                            entity,
                            CRigidBody{
                                .mass = 0.f
                            });
                    }

                    if (ImGui::MenuItem("Dynamic"))
                    {
                        ecs.AddComponent<CRigidBody>(
                            entity,
                            CRigidBody{
                                .mass = 1.f
                            });
                    }
                }

                if (multipleChoiceComponents[i] == "Light" && !ecs.HasComponent<CLight>(entity))
                {
                    if (ImGui::MenuItem("Point"))
                    {
                        ecs.AddComponent<CLight>(
                            entity,
                            CLight{
                                .type = POINT,
                                .color = glm::vec3(1.0f),
                                .radius = 8.0f,
                                .strength = 1.0f,
                                .shadowCaster = false,
                                .offset = glm::vec3(0.000f, 0.000f, 0.000f)
                            });
                    }

                    if (ImGui::MenuItem("Spot"))
                    {
                        ecs.AddComponent<CLight>(
                            entity,
                            CLight{
                                .type = SPOT,
                                .color = glm::vec3(1.0f),
                                .radius = 9.0f,
                                .strength = 1.0f,
                                .direction = glm::vec3(0.0f, 0.0f, 0.0f),
                                .innerCutoff = 12.5f,
                                .outerCutoff = 17.5f,
                            });
                    }

                    if (ImGui::MenuItem("Directional"))
                    {
                        ecs.AddComponent<CLight>(
                            entity,
                            CLight{
                                .type = DIRECTIONAL,
                                .color = glm::vec3(1.0f),
                                .strength = 1.0f, 
                            });
                    }
                }

                if (multipleChoiceComponents[i] == "Model" && !ecs.HasComponent<CModel>(entity))
                {
                    std::vector<std::string> modelNames = AssetManager::GetModelNames();

                    for (int i = 0; i < modelNames.size(); i++)
                    {
                        if (ImGui::MenuItem(modelNames[i].c_str()))
                        {
                            ecs.AddComponent<CModel>(
                                entity,
                                CModel{
                                    .model = AssetManager::GetModel(modelNames[i]),
                                    .modelName = modelNames[i]
                                });
                        }

                    }
                }
                ImGui::EndMenu();
            }
        }

        ImGui::EndPopup();
    }
    else if (singleChoiceComponents[selectedComponent] == "Transform" && !ecs.HasComponent<CTransform>(entity))
    {

        const auto& playerFront = ecs.GetComponent<CPlayer>(playerEntity).front;
        auto& playerPosition = ecs.GetComponent<CTransform>(playerEntity).position;

        ecs.AddComponent<CTransform>(
            entity,
            CTransform{
                .position = playerPosition + playerFront * 2.0f,
                .scale = glm::vec3(1.0f),
                .rotation = glm::quat(),
            });

        selectedComponent = -1;
    }


    if (!addingNewEntity)
    {
        if (ImGui::Button("Destroy Entity"))
        {
            ecs.DestroyEntity(entity);
        }
    }
  
    if (ImGui::Button("Duplicate"))
    {
        Util::duplicateEntity(entity);
    }
}