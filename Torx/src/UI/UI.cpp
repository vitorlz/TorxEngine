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
#include "../Physics/Raycast.h"
#include <iomanip>
#include <sstream>

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

    // Gizmos
    
    static int selectedEntity{ -1 };
    
    if (!ImGuizmo::IsOver())
    {
        selectedEntity = Raycast::getSelectedEntity();
    }
   
    if (selectedEntity >= 0)
    {
        ImGuizmo::SetOrthographic(false);

        ImGuizmo::SetRect(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT);

        auto& transform = ecs.GetComponent<CTransform>(selectedEntity);
        auto& rigidbody = ecs.GetComponent<CRigidBody>(selectedEntity);

        // you need to create a camera component and give it to the player and store the projection and view matrix inside it.
        // the way we are getting the view and projection matrix right now is just ugly.

        glm::mat4 view = Common::playerViewMatrix;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        glm::mat4 rotMatrix = glm::mat4_cast(glm::quat(glm::vec3(glm::radians(transform.rotation.x), glm::radians(transform.rotation.y), glm::radians(transform.rotation.z))));
        model *= rotMatrix;
        model = glm::scale(model, transform.scale);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f), (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT, 0.1f, 200.0f);

        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(model));

        if (ImGuizmo::IsUsing())
        {
            Common::usingGuizmo = true;
            transform.position = glm::vec3(model[3]);

            btTransform physicsTransform;

            physicsTransform.setIdentity();

            physicsTransform.setOrigin(btVector3(
                btScalar(transform.position.x),
                btScalar(transform.position.y),
                btScalar(transform.position.z)));

            btQuaternion quatRot(glm::radians(transform.rotation.y), glm::radians(transform.rotation.x), glm::radians(transform.rotation.z));

            physicsTransform.setRotation(quatRot);
            
            rigidbody.body->getMotionState()->setWorldTransform(physicsTransform);
            rigidbody.body->setWorldTransform(physicsTransform);
        }
        else 
        {
            Common::usingGuizmo = false;
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


