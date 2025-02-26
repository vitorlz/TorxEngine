#include <glad/glad.h>
#include "UI.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "Rendering/RenderingUtil.h"
#include "Core/Common.h"
#include "Core/Coordinator.hpp"
#include "AssetLoading/AssetManager.h"
#include "Components/CLight.h"
#include "Components/CModel.h"
#include "Components/CTransform.h"
#include "Components/CSingleton_Input.h"
#include "Components/CPlayer.h"
#include "Components/CRigidBody.h"
#include "Components/CMesh.h"
#include "Components/CAnimator.h"
#include "Components/CCamera.h"
#include "Components/CNativeScript.h"
#include "Physics/Raycast.h"
#include "../Editor/Editor.h"
#include "Scene/Scene.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "Util/Util.h"
#include "Engine.h"
#include "Util/ShaderManager.h"
#include "../Editor/EditorCamera.h"
#include "Scene/Scene.h"
#include "Util/WindowsPlatform/WindowsUtil.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <filesystem>

#include <IconsFontAwesome4.h>

glm::vec2 UI::gameWindowMousePos;
glm::vec2 UI::gameWindowSize;
glm::vec2 UI::gameWindowPos;

bool UI::isOpen{ true };
bool UI::firstMouseUpdateAfterMenu{ false };
bool UI::hovering{ false };
bool UI::spectatingCamera{ false };

bool UI::projectLoaded{ false };

void showComponents(Entity entity);
void showEntityOptions(Entity entity, bool addingNewEntity);
std::string findGameCMakeDir();
extern Coordinator ecs;

Entity playerEntity;

void UI::Init(GLFWwindow* window) 
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
   
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    io.Fonts->AddFontDefault();
    float baseFontSize = 21.0f;
    float iconFontSize = baseFontSize * 2.0f / 3.0f;

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = iconFontSize;
    std::cout << "icon fontsize: " << iconFontSize << "\n";
    icons_config.GlyphOffset.y = 2;
    io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FA, iconFontSize, &icons_config, icons_ranges);
}

void UI::NewFrame() 
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

Entity addingFromEntityList;
void UI::Update() 
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    ImGui::DockSpaceOverViewport(
        dockspace_id,
        ImGui::GetMainViewport(),
        ImGuiDockNodeFlags_PassthruCentralNode,
        nullptr 
    );

    // dock inspector, settings, and game windows to their starting positions

    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);
      
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        ImGuiID left_dock_id = ImGui::DockBuilderSplitNode(
            dockspace_id,          
            ImGuiDir_Left,         
            0.25f,                 
            nullptr,               
            &dockspace_id          
        );

        ImGuiID left_bottom_id;
        ImGuiID left_top_id = ImGui::DockBuilderSplitNode(
            left_dock_id,
            ImGuiDir_Up,
            0.5f,
            nullptr,           
            &left_bottom_id    
        );

        ImGui::DockBuilderDockWindow("Settings", left_top_id);
        ImGui::DockBuilderDockWindow("Inspector", left_bottom_id);
        ImGui::DockBuilderDockWindow("Game", dockspace_id);
      
        ImGui::DockBuilderFinish(dockspace_id);
    }

    RenderGameWindow();

    bool menuWindowHovered =  false;
    bool editorWindowHovered = false;

    ImGui::Begin("Settings");
    menuWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

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

            if (std::filesystem::is_directory(path))
            {
                for (const auto& entry : std::filesystem::directory_iterator(path))
                {
                    envMaps.push_back(entry.path().filename().string());
                }
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
        ImGui::SliderFloat("Voxelization area size", &Common::voxelizationAreaSize, 0.0f, 150.0f);

        ImGui::SeparatorText("Indirect Diffuse Lighting");
        ImGui::Checkbox("Show indirect diffuse light accumulation", &Common::showDiffuseAccumulation);
        ImGui::Checkbox("Show total indirect diffuse light", &Common::showTotalIndirectDiffuseLight);
        ImGui::SliderFloat("Diffuse cone spread", &Common::diffuseConeSpread, 0.001f, 100.0f);

        ImGui::SeparatorText("Indirect specular lighting");
        ImGui::SliderFloat("Specular bias", &Common::vxSpecularBias, 0.0f, 100.0f);
        ImGui::SliderFloat("Cone origin offset", &Common::specularConeOriginOffset, 0.0f, 20.0f);
        ImGui::SliderFloat("Cone max. distance", &Common::specularConeMaxDistance, 0.1f, 20.0f);
        ImGui::SliderFloat("Step size multiplier", &Common::specularStepSizeMultiplier, 0.1f, 10.0f);
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

    if (ImGui::TreeNode("SSR"))
    {
        ImGui::SliderFloat("Max. Distance", &Common::ssrMaxDistance, 0.0f, 100.0f);
        ImGui::SliderFloat("Resolution", &Common::ssrResolution, 0.0f, 1.0f);
        ImGui::SliderFloat("Thickness", &Common::ssrThickness, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular Bias", &Common::ssrSpecularBias, 0.0f, 100.0f);
        ImGui::SliderInt("Steps", &Common::ssrSteps, 0, 100);
        ImGui::SliderFloat("Max. Blur Distance", &Common::ssrMaxBlurDistance, 0.0f, 100.0f);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("SSAO"))
    {
        ImGui::Checkbox("SSAO on", &Common::ssaoOn);
        ImGui::Checkbox("Show SSAO", &Common::showAO);
        ImGui::SliderFloat("Max. Distance", &Common::ssaoRadius, 0.0f, 10.0f);
        ImGui::SliderFloat("Strength", &Common::ssaoPower, 0.0f, 10.0f);
        if (ImGui::SliderInt("Kernel Size", &Common::ssaoKernelSize, 0, 200))
        {
            RenderingUtil::CreateSSAOKernel(Common::ssaoKernelSize);
        };

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Text Rendering"))
    {
        ImGui::ColorEdit3("Text Color", &Common::textColor.x);

        ImGui::TreePop();
    }

    ImGui::End();

    // Gizmos
    ImGui::Begin("Inspector");
    editorWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

    static int selectedEntity{ -1 };
    static bool addingNewEntity{ false };
    static Entity newEntity;

    if (Torx::Engine::MODE == Torx::EDITOR)
    {
       
        if (!addingNewEntity && inputSing.rightMousePressed)
        {
            glm::vec4 rayStartNDC(
                (UI::gameWindowMousePos.x / (double)UI::gameWindowSize.x - 0.5f) * 2.0f,
                (((double)UI::gameWindowSize.y - UI::gameWindowMousePos.y) / (double)UI::gameWindowSize.y - 0.5f) * 2.0f,
                -1.0f,
                1.0f
            );

            glm::vec4 rayEndNDC(
                (UI::gameWindowMousePos.x / (double)UI::gameWindowSize.x - 0.5f) * 2.0f,
                (((double)UI::gameWindowSize.y - UI::gameWindowMousePos.y) / (double)UI::gameWindowSize.y - 0.5f) * 2.0f,
                1.0f,
                1.0f
            );

           // ImVec2 mousepos = ImGui::GetMousePos();
            selectedEntity = Raycast::mouseRaycast(Common::currentProjMatrix * Common::currentViewMatrix, rayStartNDC, rayEndNDC, 1000.0f);
            //std::cout << "Mouse Pos game: " << gameWindowMousePos.x << ", " << gameWindowMousePos.y << std::endl;
            //std::cout << "Mouse pos imgui: " << mousepos.x << ", " << mousepos.y << std::endl;
        }

        if (selectedEntity >= 0 && ecs.isAlive(selectedEntity) && !spectatingCamera)
        {
            Editor::getInstance().RenderGizmo(selectedEntity);
        }
    }

    if (!addingNewEntity && selectedEntity >= 0 && ecs.isAlive(selectedEntity))
    {
                
        std::stringstream ss;
        ss << "Selected Entity: " << selectedEntity;
        std::string selectedEntityText = ss.str();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), selectedEntityText.c_str());
        showComponents(selectedEntity);
        showEntityOptions(selectedEntity, addingNewEntity);
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

    if (menuWindowHovered || editorWindowHovered || ImGui::IsAnyItemHovered())
    {
        hovering = true;
    }
    else 
    {
        hovering = false;
    }

    if (projectsDir.empty())
    {
        findProjectsDir();
    }

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
      
            // essentially by "load project" we mean simply setting the working directory to the project's folder so we can access its resources folder.
            if (ImGui::BeginMenu("Load project"))
            {
                std::unordered_map<std::string, std::filesystem::path> projects;
               
                if(!projectsDir.empty())
                {
                    for (const auto& j : std::filesystem::directory_iterator(projectsDir))
                    {
                        projects.insert({ j.path().filename().string(), j.path() });
                    }
                }
              
                if (!projects.empty())
                {
                    auto it = projects.begin();
                    for (size_t i = 0; i < projects.size(); i++)
                    {
                        if (ImGui::MenuItem(it->first.c_str()))
                        {
                            std::filesystem::current_path(it->second);

                            // if a project was already loaded, reload shaders when switching projects
                            if (projectLoaded)
                            {
                                ShaderManager::ReloadShaders();
                            }
                            bool scenefound = false;
                            // load one of the scenes when loading a new project
                            std::string path = "res/scenes";
                            if (std::filesystem::exists(path))
                            {
                                for (const auto& i : std::filesystem::directory_iterator(path))
                                {
                                    for (Entity e : ecs.GetLivingEntities())
                                    {
                                        ecs.DestroyEntity(e);
                                    }
                                    ecs.ResetEntityIDs();
                                    ECSCore::UpdateSystems(0.0f);
                                    Scene::LoadSceneFromJson(i.path().string());
                                    Common::voxelize = true;

                                    scenefound = true;

                                    break;
                                }

                                if (!scenefound)
                                {

                                    std::cout << "No scenes were found\n";

                                    for (Entity e : ecs.GetLivingEntities())
                                    {
                                        ecs.DestroyEntity(e);
                                    }
                                    ecs.ResetEntityIDs();
                                    ECSCore::UpdateSystems(0.0f);
                                    Scene::SetEnvironmentMap("");
                                    RenderingUtil::LoadNewEnvironmentMap("");
                                    Scene::g_currentScenePath = "";
                                    Common::voxelize = true;
                                }
                            }
                            
                            ShaderManager::ReloadShaders();
                            std::cout << "PROJECT LOADED!" << "\n";
                            projectLoaded = true;

                            break;
                        }

                        it++;
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("New project..."))
            {
                openChooseProjectTitlePopup = true;
            }

            ImGui::EndMenu();
            
        }

        if (!projectLoaded)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::BeginMenu("Project"))
        {
            
            if (ImGui::BeginMenu("Load scene"))
            {
                std::unordered_map<std::string, std::filesystem::path> scenes;
                std::string path = "res/scenes";

                if (std::filesystem::exists(path))
                {
                    for (const auto& i : std::filesystem::directory_iterator(path))
                    {
                        scenes.insert({ i.path().filename().string(), i.path() });
                    }
                }

                if (!scenes.empty())
                {
                    auto it = scenes.begin();
                    for (size_t i = 0; i < scenes.size(); i++)
                    {
                        if (ImGui::MenuItem(it->first.c_str()))
                        {

                            for (Entity e : ecs.GetLivingEntities())
                            {
                                ecs.DestroyEntity(e);
                            }
                            ecs.ResetEntityIDs();
                            ECSCore::UpdateSystems(0.0f);
                            Scene::LoadSceneFromJson(it->second.string());
                            Common::voxelize = true;
                            ShaderManager::ReloadShaders();

                            break;
                        }

                        it++;
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("New scene..."))
            {
                openChooseScenePopup = true;
            }

            if (ImGui::MenuItem("Save scene"))
            {
                if (!Scene::g_currentScenePath.empty())
                {
                    if (!std::filesystem::exists(Scene::g_currentScenePath))
                    {
                        std::cout << "New scene created!\n";
                        std::ofstream ofs(Scene::g_currentScenePath);
                        ofs.close();
                    }

                    Scene::SaveSceneToJson(Scene::g_currentScenePath);
                }
                else
                {
                    openChooseScenePopup = true;
                    savingUnnamedScene = true;
                }
            }
            if (ImGui::MenuItem("Save scene as..."))
            {
                std::string filePath = FileDialogs::SaveFile("Json files (*.json)\0*.json\0");

                if (!filePath.empty())
                {
                    Scene::SaveSceneToJson(filePath);
                }
            }

            ImGui::EndMenu();
        }

        if (!projectLoaded)
        {
            ImGui::EndDisabled();
        }


        ImGui::EndMainMenuBar();
    }

    // we have to openg this popup from outside the menu's scope because imgui menu's and widgets have different lifetimes.
    if (openChooseScenePopup)
    {
        ImGui::OpenPopup("Choose scene title");
        openChooseScenePopup = false;
    }

    if (ImGui::BeginPopupModal("Choose scene title", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char fileName[32] = "";
        std::filesystem::path path;
        static bool fileAlreadyExists = false;

        ImGui::InputText("", fileName, 32);
        path = "res/scenes";
        path /= std::string(fileName) + ".json";
        //std::cout << "New save path: " << path.string() << "\n";
        if (std::filesystem::is_regular_file(path))
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Title already exists, please choose a new one.");
            fileAlreadyExists = true;
        }
        else
        {
            fileAlreadyExists = false;
        }

        if (ImGui::Button("Ok"))
        {
            if (!fileAlreadyExists && !savingUnnamedScene)
            {
                if (!Scene::g_currentScenePath.empty())
                {
                    for (Entity e : ecs.GetLivingEntities())
                    {
                        ecs.DestroyEntity(e);
                    }
                    ecs.ResetEntityIDs();
                    ECSCore::UpdateSystems(0.0f);
                    Scene::SetEnvironmentMap("");
                    RenderingUtil::LoadNewEnvironmentMap("");
                    Scene::g_currentScenePath = path.string();
                    Common::voxelize = true;
                }
                else
                {
                    Scene::g_currentScenePath = path.string();
                }
            }
            else if (!fileAlreadyExists && savingUnnamedScene)
            {
                Scene::g_currentScenePath = path.string();
                Scene::SaveSceneToJson(Scene::g_currentScenePath);
                savingUnnamedScene = false;
            }

            ImGui::CloseCurrentPopup();
        }
       

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (openChooseProjectTitlePopup)
    {
        ImGui::OpenPopup("Choose project title");
        openChooseProjectTitlePopup = false;
    }

    static bool newProjectCreated = false;
    static std::string newProjectPath = "";

    if (ImGui::BeginPopupModal("Choose project title", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char projectName[32] = "";
        std::filesystem::path path;
        static bool projectAlreadyExists = false;

        ImGui::InputText("", projectName, 32);
        path = std::string(PROJECT_SOURCE_DIR) + "/Projects" ;
        path /= std::string(projectName);
        //std::cout << "New save path: " << path.string() << "\n";
        if (std::filesystem::is_directory(path) && !std::string(projectName).empty())
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Title already exists, please choose a new one.");
            projectAlreadyExists = true;
        }
        else if (std::string(projectName).empty())
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Project title cannot be empty");
            projectAlreadyExists = true;
        }
        else
        {
            projectAlreadyExists = false;
        }

        if (ImGui::Button("Ok"))
        {
            if (!projectAlreadyExists)
            {
                std::string command = "python " + std::string(PROJECT_SOURCE_DIR) + "/create_new_project.py " + projectName;
                int result = std::system(command.c_str());

                if (result == 0)
                {
                    newProjectCreated = true;
                    newProjectPath = projectsDir + "\\" + projectName;

                    for (Entity e : ecs.GetLivingEntities())
                    {
                        ecs.DestroyEntity(e);
                    }
                    ecs.ResetEntityIDs();
                    ECSCore::UpdateSystems(0.0f);
                    Scene::SetEnvironmentMap("");
                    RenderingUtil::LoadNewEnvironmentMap("");
                    Scene::g_currentScenePath = "";
                    Common::voxelize = true;

                    std::cout << "New project path: " << newProjectPath << "\n";
                    std::cout << "Project created successfully\n";
                }
                else
                {
                    std::cout << "Error creating project\n";
                }

                std::cout << "command: " << command << "\n";
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (newProjectCreated && std::filesystem::is_directory(newProjectPath + "/res") && !newProjectPath.empty())
    {
        std::filesystem::current_path(newProjectPath);
        projectLoaded = true;

        newProjectCreated = false;

        std::cout << "Changed current path!" << "\n";
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::RenderGameWindow()
{

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // No padding inside the window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("Game");

    ImGui::PopStyleVar(2);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Padding

    ImVec2 gameSize = ImGui::GetContentRegionAvail();
    ImGui::Image(RenderingUtil::gGameWindowTexture, gameSize, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::PopStyleVar();

    ImGui::SetNextItemAllowOverlap();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.93f, 0.93f, 0.93f, 0.3f));  // Change button color
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));  // Hover color
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1803921569f, 0.262745098f, 0.4549019608f, 1.0f));  // Click color
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);  // Rounded corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f); // Border thickness
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 7)); // Padding

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 windowPos = ImGui::GetWindowPos();

    float iconsize = 14.2f;

    float offset = 30 + iconsize;

    ImVec2 buttonPos = ImVec2((windowSize.x - offset * 2) * 0.5f, 40.0f);

    // Manually set cursor to draw button OVER the texture
    ImGui::SetCursorPos(buttonPos);

    if (ImGui::Button(ICON_FA_PAUSE))
    {
        // Play button clicked
    }

    ImGui::PopStyleColor(3);

    buttonPos = ImVec2((windowSize.x ) * 0.5f, 40.0f); 

    if (Torx::Engine::MODE == Torx::PLAY)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1803921569f, 0.262745098f, 0.4549019608f, 1.0f));
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.93f, 0.93f, 0.93f, 0.3f));
    }

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));  // Hover color
    
   // Change button color

    // Manually set cursor to draw button OVER the texture
    ImGui::SetCursorPos(buttonPos);

    if (ImGui::Button(ICON_FA_PLAY))
    {
        if (Torx::Engine::MODE == Torx::EDITOR)
        {
            Scene::g_editorScene = Scene::SerializeScene();
            Torx::Engine::MODE = Torx::PLAY;
        }
    }

    ImGui::PopStyleColor(2);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.93f, 0.93f, 0.93f, 0.3f));  // Change button color
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));  // Hover color
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1803921569f, 0.262745098f, 0.4549019608f, 1.0f));  // Click color


    buttonPos = ImVec2((windowSize.x + offset * 2) * 0.5f, 40.0f); // Top center

    // Manually set cursor to draw button OVER the texture
    ImGui::SetCursorPos(buttonPos);

    if (ImGui::Button(ICON_FA_STOP))
    {
        if (Torx::Engine::MODE == Torx::PLAY)
        {
            for (Entity e : ecs.GetLivingEntities())
            {
                ecs.DestroyEntity(e);
            }
            ecs.ResetEntityIDs();
            ECSCore::UpdateSystems(0.0f);

            Torx::Engine::MODE = Torx::EDITOR;
            Scene::DeserializeScene(Scene::g_editorScene);

            Window::ShowCursor();
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(3);

    ImVec2 game_window_pos = ImGui::GetWindowPos();
    ImVec2 game_window_size = ImGui::GetWindowSize();
    gameWindowPos = { game_window_pos.x, game_window_pos.y };
    gameWindowSize = { game_window_size.x, game_window_size.y };
    ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);
    

    // Check if the mouse is hovering the game window
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
    {
        // Get mouse position relative to the game window
        ImVec2 mouse_pos = ImGui::GetMousePos();

        // subtract 19 pixels from vertical position because of imgui's window header size
        gameWindowMousePos = { mouse_pos.x - game_window_pos.x, mouse_pos.y - (game_window_pos.y + 19) };
    }

    ImGui::End();
}

void UI::Terminate() 
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UI::showComponents(Entity entity)
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
        if (ImGui::Button("Delete##xx5"))
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

    if (ecs.HasComponent<CAnimator>(entity))
    {
        if (ImGui::CollapsingHeader("Animator Component", ImGuiTreeNodeFlags_AllowItemOverlap))
        {
            Animator& animator = ecs.GetComponent<CAnimator>(entity).animator;
            Animation* animation = animator.GetCurrentAnimation();

            float duration = animation->GetDuration();
            float currentTime = fmod(animator.GetCurrentTime(), duration);
            float ticksPerSecond = animation->GetTicksPerSecond();
            
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), animation->GetAnimationPath().c_str());
            ImGui::InputFloat("Duration", &duration);
            ImGui::InputFloat("Ticks per second", &ticksPerSecond);
            ImGui::InputFloat("Current Time", &currentTime);
            ImGui::ProgressBar(currentTime / duration, ImVec2(0.0f, 0.0f));
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete##xx6"))
        {
            ecs.RemoveComponent<CAnimator>(entity);
        }
    }

    if (ecs.HasComponent<CCamera>(entity))
    {
        if (ImGui::CollapsingHeader("Camera Component", ImGuiTreeNodeFlags_AllowItemOverlap))
        {
            bool justChanged = false;

            CCamera& cameraComp = ecs.GetComponent<CCamera>(entity);

            const char* items[] = { "Perspective", "Orthographic" };
            static int item_selected_idx = 0;
            const char* combo_preview_value = items[item_selected_idx];

            if (ImGui::BeginCombo("Projection", combo_preview_value))
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    ImGui::PushID(n);
                    const bool is_selected = (item_selected_idx == n);
                    if (ImGui::Selectable(items[n], is_selected))
                    {
                        item_selected_idx = n;

                        if (items[n] == "Perspective")
                        {
                            cameraComp.projType = PERSPECTIVE;
                            justChanged = true;
                        }
                        if (items[n] == "Orthographic")
                        {
                            
                            std::cout << "orthographic" << "\n";
                            cameraComp.projType = ORTHO;
                            justChanged = true;
                        }
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();

                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }

            // ability to change the values here

            if (cameraComp.projType == PERSPECTIVE)
            {
                if (justChanged)
                {
                   cameraComp.fov = 45.0f;
                   cameraComp.near = 0.1f;
                   cameraComp.far = 100.0f;
                   cameraComp.projection = glm::perspective(glm::radians(cameraComp.fov), (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT, cameraComp.near, cameraComp.far);
                }
                bool userInput = ImGui::InputFloat("Fov", &cameraComp.fov, 0.01f, 90.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue) ||
                    ImGui::InputFloat("Near", &cameraComp.near, 0.01f, 5.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue) ||
                    ImGui::InputFloat("Far", &cameraComp.far, 1.0f, 1000.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);

                if (userInput)
                {
                    std::cout << "fov: " << cameraComp.fov << "\n";
                    cameraComp.projection = glm::perspective(glm::radians(cameraComp.fov), (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT, cameraComp.near, cameraComp.far);
                }
            }
            else if (cameraComp.projType == ORTHO)
            {
                if (justChanged)
                {
                    cameraComp.near = 0.1f;
                    cameraComp.far = 100.0f;
                    cameraComp.left = -20;
                    cameraComp.right = 20;
                    cameraComp.bottom = -20;
                    cameraComp.top = 20;
                    cameraComp.projection = glm::ortho(cameraComp.left, cameraComp.right, cameraComp.bottom, cameraComp.top, cameraComp.near, cameraComp.far);
                }

                bool userInput = ImGui::InputFloat("Left", &cameraComp.left, 0.01f, 90.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue) ||
                    ImGui::InputFloat("Right", &cameraComp.right, 0.01f, 90.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue) ||
                    ImGui::InputFloat("Bottom", &cameraComp.bottom, 0.01f, 90.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue) ||
                    ImGui::InputFloat("Top", &cameraComp.top, 0.01f, 90.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue) ||
                    ImGui::InputFloat("Near", &cameraComp.near, 0.01f, 5.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue) ||
                    ImGui::InputFloat("Far", &cameraComp.far, 1.0f, 1000.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);

                if (userInput)
                {
                    cameraComp.projection = glm::ortho(cameraComp.left, cameraComp.right, cameraComp.bottom, cameraComp.top, cameraComp.near, cameraComp.far);
                }
            }

            if (ImGui::Checkbox("Spectate", &spectatingCamera))
            {
            }

            if (spectatingCamera && ecs.HasComponent<CTransform>(entity))
            {   
                CTransform& transform = ecs.GetComponent<CTransform>(entity);
                Common::currentProjMatrix = cameraComp.projection;

                glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position);
                model = model * glm::mat4_cast(transform.rotation);

                Common::currentViewMatrix = inverse(model);
                Common::currentCamPos = transform.position;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete##xx7"))
        {
            ecs.RemoveComponent<CCamera>(entity);
        }
    }


    if (ecs.HasComponent<CNativeScript>(entity))
    {

        auto& scriptComponent = ecs.GetComponent<CNativeScript>(entity);
        if (ImGui::CollapsingHeader("Script Component", ImGuiTreeNodeFlags_AllowItemOverlap))
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), scriptComponent.name.c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete##xx8"))
        {
            delete scriptComponent.script;
            ecs.RemoveComponent<CNativeScript>(entity);
        }
    }

}

void showEntityOptions(Entity entity, bool addingNewEntity)
{
    static int selectedComponent = -1;
    const char* singleChoiceComponents[] = { "Transform", "Player", "Camera" };
    const char* multipleChoiceComponents[] = { "Mesh", "Model", "Rigid body", "Light", "Script" };

    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("my_select_popup");
    
    if (ImGui::BeginPopup("my_select_popup"))
    {
        ImGui::SeparatorText("Select a component");
 
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
                    std::unordered_map<std::string, std::filesystem::path> models;
                    std::string path = "res/models";

                    if (std::filesystem::exists(path))
                    {
                        for (const auto& i : std::filesystem::directory_iterator(path))
                        {
                            models.insert({ i.path().filename().string(), i.path() });
                        }
                    }

                    if (!models.empty())
                    {
                        auto it = models.begin();
                        for (size_t i = 0; i < models.size(); i++)
                        {
                            if (ImGui::MenuItem(it->first.c_str()))
                            {
                                // we always want to store relative paths

                                std::string modelName = it->first.c_str();

                                for (const auto& i : std::filesystem::directory_iterator(it->second))
                                {

                                    if (i.path().filename().extension() == ".gltf")
                                    {
                                        models.insert({ i.path().filename().string(), i.path() });

                                        std::string relativePath = i.path().relative_path().string();

                                        // replace backslashes with forward slashes
                                        std::replace(relativePath.begin(), relativePath.end(), '\\', '/');

                                        AssetManager::LoadModel(relativePath, modelName);

                                        ecs.AddComponent<CModel>(
                                            entity,
                                            CModel{
                                                .model = AssetManager::GetModel(modelName),
                                                .modelName = modelName,
                                                .path = relativePath,
                                            });

                                        break;
                                    }

                                }

                                break;
                            }

                            it++;
                        }
                    } 
                }

                if (multipleChoiceComponents[i] == "Script" && !ecs.HasComponent<CNativeScript>(entity))
                {
                    std::vector<std::string> scriptNames = ScriptFactory::GetNames();

                    for (int i = 0; i < scriptNames.size(); i++)
                    {
                        if (ImGui::MenuItem(scriptNames[i].c_str()))
                        {
                            ecs.AddComponent<CNativeScript>(
                                entity,
                                CNativeScript{
                                    .name = scriptNames[i],
                                });
                        }
                    }
                }

                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < IM_ARRAYSIZE(singleChoiceComponents); i++)
        {
            if (ImGui::Selectable(singleChoiceComponents[i]))
            {
                if (singleChoiceComponents[i] == "Transform" && !ecs.HasComponent<CTransform>(entity))
                {
                    EditorCamera& editorCamera = Editor::getInstance().GetEditorCamera();

                    ecs.AddComponent<CTransform>(
                        entity,
                        CTransform{
                            .position = editorCamera.GetTransform().position + editorCamera.GetFront() * 2.0f,
                            .scale = glm::vec3(1.0f),
                            .rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                        });

                    selectedComponent = -1;
                }
                else if (singleChoiceComponents[i] == "Player" && !ecs.HasComponent<CPlayer>(entity))
                {
                    ecs.AddComponent<CPlayer>(
                        entity,
                        CPlayer{
                            .flashlightOn = false,
                            .movementSpeed = 3.0f,
                        });
                }
                else if (singleChoiceComponents[i] == "Camera" && !ecs.HasComponent<CCamera>(entity))
                {
                    std::cout << "adding camera to entity " << entity << "\n";
                    std::cout << "camera selected";

                    // add camera with some default values here.
                    ecs.AddComponent<CCamera>(
                        entity,
                        CCamera{
                           .projType = PERSPECTIVE,
                           .projection = glm::perspective(glm::radians(45.0f), (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT, 0.01f,  100.0f),
                           .fov = 45.0f,
                           .near = 0.1f,
                           .far = 100.0f,
                           .left = -20,
                           .right = 20,
                           .bottom = -20,
                           .top = 20,
                        });
                }
            }
        }

        ImGui::EndPopup();
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

void UI::findProjectsDir()
{
    std::string path = "../";

    // if there is already a project loaded, keep searching for projects from the editor root directory

    // go up the file hierarchy until we find the Projects folder, then get all project names
    if (projectsDir.empty())
    {
        if (std::filesystem::exists(path))
        {
            for (const auto& i : std::filesystem::directory_iterator(path))
            {
                if (i.path().filename().string() == "Projects")
                {
                    projectsDir = std::filesystem::absolute(i.path()).string();
                }

                path = path + "..";
                if (!std::filesystem::exists(path))
                {
                    std::cout << "Projects folder not found" << "\n";
                    break;
                }
            }
        }
    }
}