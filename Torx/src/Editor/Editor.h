#pragma once

#include <glm/glm.hpp>

namespace Editor
{
	void RenderGizmo(int selectedEntity);
	int GetCurrentGizmoMode();
	int GetCurrentGizmoOperation();
	void SetCurrentGizmoMode(int gizmoMode);
	void SetCurrentGizmoOperation(int gizmoOperation);
}