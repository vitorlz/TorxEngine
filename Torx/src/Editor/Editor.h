#pragma once

#include "imgui.h"
#include "ImGuizmo.h"
#include <glm/glm.hpp>
#include "EditorCamera.h"
#include <vector>
#include "../UI/UI.h"

class Editor
{
public:
	Editor(Editor const&) = delete;
	void operator = (Editor const&) = delete;
	static Editor& getInstance();

	void InitUI();
	void RenderGizmo(int selectedEntity);
	int GetCurrentGizmoMode();
	int GetCurrentGizmoOperation();
	void SetCurrentGizmoMode(int gizmoMode);
	void SetCurrentGizmoOperation(int gizmoOperation);
	bool isOn();
	void setStatus(bool status);
	void Update(float dt);
	EditorCamera& GetEditorCamera();
	UI& GetUI();

private:
	Editor() {};
	EditorCamera editorCamera;
	UI ui;

	ImGuizmo::MODE currentGizmoMode;
	ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;
	bool useSnap = false;
	std::vector<float> snap;
	std::vector<float> translateSnap = { 1.f, 1.f, 1.f };
	std::vector<float> rotateSnap = { 1.f, 1.f, 1.f };
	std::vector<float> scaleSnap = { 1.f, 1.f, 1.f };
	float bounds[6] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	float boundsSnap[6] = { 0.1f, 0.1f, 0.1f };
	bool boundSizing = false;
	bool boundSizingSnap = false;
	bool editorOn = false;
};