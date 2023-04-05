#pragma once
#include "imgui.h"

void displaySceneObject(Ref<Scene> scene, uint32_t sceneObjectId, int32_t& selectedObject)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
	Ref<SceneObject> sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);

	if (selectedObject == sceneObject->GetId())
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	if (sceneObject->GetChildEntities().size() > 0)
	{
		bool nodeOpen = ImGui::TreeNodeEx(sceneObject->GetEntityName()->c_str(), nodeFlags);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			selectedObject = sceneObject->GetId();
		if (nodeOpen)
		{
			for (Ref<Entity> entity : sceneObject->GetChildEntities())
			{
				displaySceneObject(scene, entity->GetId(), selectedObject);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx(sceneObject->GetEntityName()->c_str(), nodeFlags);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			selectedObject = sceneObject->GetId();
	}
}

void BuildSceneHierarchy(Ref<Scene> scene, int32_t& selectedSceneObject)
{
	ImGui::Begin("Scene Hierarchy", 0, ImGuiWindowFlags_NoCollapse);
	int32_t selectedObject = selectedSceneObject;
	for (uint32_t sceneObjectId : scene->GetSceneObjectIds())
	{
		displaySceneObject(scene, sceneObjectId, selectedObject);
	}
	selectedSceneObject = selectedObject;
	ImGui::End();
}