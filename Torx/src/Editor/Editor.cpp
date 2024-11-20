#include "Editor.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "../Core/Common.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CTransform.h"
#include "../Components/CRigidBody.h"
#include "../Components/CSingleton_Input.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


extern Coordinator ecs;

namespace Editor
{
    ImGuizmo::MODE currentGizmoMode;
    ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;

    CSingleton_Input& inputSing = CSingleton_Input::getInstance();

	void RenderGizmo(int selectedEntity)
	{

        if (!ecs.HasComponent<CTransform>(selectedEntity))
        {
            std::cout << "Can't render gizmo: entity has no transform" << "\n";
            return;
        }

		ImGuizmo::SetOrthographic(false);

		ImGuizmo::SetRect(0, 0, Common::SCR_WIDTH, Common::SCR_HEIGHT);

		auto& transform = ecs.GetComponent<CTransform>(selectedEntity);

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

        if (inputSing.pressedKeys[NUMBER_1])
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        if (inputSing.pressedKeys[NUMBER_2])
            currentGizmoOperation = ImGuizmo::ROTATE;
        if (inputSing.pressedKeys[NUMBER_3]) // r Key
            currentGizmoOperation = ImGuizmo::SCALE;

        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), currentGizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(model));

        if (ImGuizmo::IsUsing())
        {
            Common::usingGuizmo = true;

            // update graphics transform

            glm::vec3 translation, rotation, scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));

            transform.position = translation;
            transform.rotation = rotation;
            transform.scale = scale;
        }
        else
        {
            Common::usingGuizmo = false;
        }
	}

    int GetCurrentGizmoMode()
    {
        return currentGizmoMode;
    }
    int GetCurrentGizmoOperation()
    {
        return currentGizmoOperation;
    }

    void SetCurrentGizmoMode(int gizmoMode)
    {
        currentGizmoMode = (ImGuizmo::MODE)gizmoMode;
    }
    void SetCurrentGizmoOperation(int gizmoOperation)
    {
        currentGizmoOperation = (ImGuizmo::OPERATION)gizmoOperation;
    }
}