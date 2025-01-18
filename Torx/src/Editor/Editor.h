#pragma once

#include <glm/glm.hpp>

namespace Editor
{
	void RenderGizmo(int selectedEntity);
	int GetCurrentGizmoMode();
	int GetCurrentGizmoOperation();
	void SetCurrentGizmoMode(int gizmoMode);
	void SetCurrentGizmoOperation(int gizmoOperation);
	bool isOn();
	void setStatus(bool status);
	void Update(float dt);
}