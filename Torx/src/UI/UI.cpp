#include "UI.h"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include "../Core/Common.h"
#include "../Core/Coordinator.hpp"
#include "../AssetLoading/AssetManager.h"
#include "../Components/CLight.h"
#include "../Components/CModel.h"
#include "../Components/CTransform.h"
#include "../Components/CSingleton_Input.h"
#include "../Components/CPlayer.h"

bool UI::isOpen{ false };
bool UI::firstMouseUpdateAfterMenu{ false };

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
}

void UI::Update() 
{
    ImGui::Begin("Menu");
    ImGui::Shortcut(ImGuiKey_Tab, ImGuiInputFlags_None);

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
                if (ecs.HasComponent<CLight>(livingEntities[i]))
                {
                    CLight& light = ecs.GetComponent<CLight>(livingEntities[i]);
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

                        ImGui::ColorEdit3("Ambient", &light.ambient.x);
                        ImGui::ColorEdit3("Diffuse", &light.diffuse.x);
                        ImGui::ColorEdit3("Specular", &light.specular.x);
                        ImGui::SliderFloat("Radius", &light.radius, 0.001f, 30.0f, "%.5f");
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
                        ecs.RemoveComponent<CLight>(livingEntities[i]);
                    }
                }
               
                if (ecs.HasComponent<CTransform>(livingEntities[i]))
                {                
                    if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_AllowItemOverlap))
                    {
                        CTransform& transform = ecs.GetComponent<CTransform>(livingEntities[i]);
                        
                        ImGui::SliderFloat3("Position", &transform.position.x, -20.0f, 20.0f, "%.3f");
                        ImGui::SliderFloat3("Scale", &transform.scale.x, -10.0f, 10.0f, "%.3f");
                        ImGui::SliderFloat3("Rotation", &transform.rotation.x, -360.0f, 360.0f, "%.3f");

                        if (ecs.HasComponent<CLight>(livingEntities[i]))
                        {
                            CLight& light = ecs.GetComponent<CLight>(livingEntities[i]);
                            light.isDirty = true;
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Delete##xx1"))
                    {
                        ecs.RemoveComponent<CTransform>(livingEntities[i]);
                    }
                }

                if (ecs.HasComponent<CModel>(livingEntities[i]))
                {
                    if (ImGui::CollapsingHeader("Model Component", ImGuiTreeNodeFlags_AllowItemOverlap))
                    {
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Has model component");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Delete##xx2"))
                    {
                        ecs.RemoveComponent<CModel>(livingEntities[i]);
                    }
                }

                if (ecs.HasComponent<CPlayer>(livingEntities[i]))
                {
                    if (ImGui::CollapsingHeader("Player Component", ImGuiTreeNodeFlags_AllowItemOverlap))
                    {
                        CPlayer& player = ecs.GetComponent<CPlayer>(livingEntities[i]);
                        
                        ImGui::InputFloat3("Front", &player.front.x);
                        ImGui::InputFloat3("Right", &player.right.x);
                        ImGui::InputFloat3("Up", &player.up.x);
                        ImGui::SliderFloat("Movement Speed", &player.movementSpeed, 1.0f, 10.0f, "%.2f");
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Delete##xx3"))
                    {
                        ecs.RemoveComponent<CPlayer>(livingEntities[i]);
                    }
                }

                if (ImGui::Button("Destroy Entity"))
                {   
                    ecs.DestroyEntity(livingEntities[i]);
                }
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

    if (ImGui::TreeNode("Debug"))
    {
        ImGui::Checkbox("Show normals", &Common::normalsDebug);
        ImGui::Checkbox("Show World Position", &Common::worldPosDebug);
        ImGui::Checkbox("Wireframe mode", &Common::wireframeDebug);

        ImGui::TreePop();
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


