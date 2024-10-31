#include "UI.h"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CLight.h"
#include "../Components/CMesh.h"
#include "../Components/CTransform.h"

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
                    if (ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_None))
                    {
                        CLight& light = ecs.GetComponent<CLight>(livingEntities[i]);

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
                        ImGui::SliderFloat("Quadratic Attenuation", &light.quadratic, 0.001f, 1.0f, "%.5f");               
                    }
                }

                if (ecs.HasComponent<CTransform>(livingEntities[i]))
                {
                    if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_None))
                    {
                        CTransform& transform = ecs.GetComponent<CTransform>(livingEntities[i]);
                        
                        ImGui::SliderFloat3("Position", &transform.position.x, 0.0f, 20.0f, "%.3f"); 
                        ImGui::SliderFloat3("Scale", &transform.scale.x, 0.0f, 10.0f, "%.3f"); 
                        ImGui::SliderFloat3("Rotation", &transform.rotation.x, 0.0f, 360.0f, "%.3f"); 
                    }
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Debug"))
    {
        
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


