#pragma once
#include "imgui.h"

void displaySceneObjectContextMenu(Ref<Scene> scene, uint32_t sceneObjectId, int32_t& selectedObjectId)
{
	if (ImGui::BeginPopupContextItem("Context Menu"))
	{
		if (ImGui::MenuItem("Add SceneObject"))
		{
			scene->AddSceneObject(sceneObjectId);
		}

		if (ImGui::MenuItem("Delete"))
		{
			scene->RemoveSceneObject(sceneObjectId);
			if (sceneObjectId == selectedObjectId)
				selectedObjectId = -1;
		}

		ImGui::EndPopup();
	}
}

void displaySceneObject(Ref<Scene> scene, uint32_t sceneObjectId, int32_t& selectedObjectId)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
	Ref<SceneObject> sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);

	if (selectedObjectId == sceneObjectId)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	if (sceneObject->GetChildEntities().size() > 0)
	{
		bool nodeOpen = ImGui::TreeNodeEx(sceneObject->GetEntityName()->c_str(), nodeFlags);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			selectedObjectId = sceneObject->GetId();
		ImGui::PushID(sceneObjectId);
		displaySceneObjectContextMenu(scene, sceneObjectId, selectedObjectId);
		ImGui::PopID();
		if (nodeOpen)
		{
			for (Ref<Entity> entity : sceneObject->GetChildEntities())
			{
				displaySceneObject(scene, entity->GetId(), selectedObjectId);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx(sceneObject->GetEntityName()->c_str(), nodeFlags);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			selectedObjectId = sceneObject->GetId();
		ImGui::PushID(sceneObjectId);
		displaySceneObjectContextMenu(scene, sceneObjectId, selectedObjectId);
		ImGui::PopID();
	}
}

void BuildSceneHierarchy(Ref<Scene> scene, int32_t& selectedSceneObjectId)
{
	ImGui::Begin("Scene Hierarchy", 0, ImGuiWindowFlags_NoCollapse);
	const char* label = "Scene";
	bool sceneOpen = ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen);
	displaySceneObjectContextMenu(scene, -1, selectedSceneObjectId);
	if(sceneOpen)
	{
		int32_t selectedObject = selectedSceneObjectId;
		for (uint32_t sceneObjectId : scene->GetSceneObjectIds())
		{
			displaySceneObject(scene, sceneObjectId, selectedObject);
		}
		selectedSceneObjectId = selectedObject;
		ImGui::TreePop();
	}
	ImGui::End();
}