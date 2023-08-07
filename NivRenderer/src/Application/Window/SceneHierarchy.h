#pragma once
#include "imgui.h"

void displaySceneObjectContextMenu(Ref<Scene> scene, uint32_t sceneObjectId, int32_t& selectedObjectId, bool allowDelete)
{
	if (ImGui::BeginPopupContextItem("Context Menu"))
	{
		if (ImGui::MenuItem("Add SceneObject"))
		{
			scene->AddSceneObject(sceneObjectId);
		}

		if (allowDelete && ImGui::MenuItem("Delete"))
		{
			scene->RemoveSceneObject(sceneObjectId);
			if (sceneObjectId == selectedObjectId)
				selectedObjectId = -1;
		}

		ImGui::EndPopup();
	}
}

void displaySceneLightContextMenu(Ref<Scene> scene, uint32_t sceneObjectId, int32_t& selectedObjectId, bool allowDelete)
{
	if (ImGui::BeginPopupContextItem("Context Menu"))
	{
		if (ImGui::MenuItem("Add Directional Light"))
		{
			scene->AddSceneDirectionalLight();
		}
        if (ImGui::MenuItem("Add Point Light"))
        {
            scene->AddPointLight();
        }

		if (allowDelete && ImGui::MenuItem("Delete"))
		{
			scene->RemoveSceneLight(sceneObjectId);
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
		displaySceneObjectContextMenu(scene, sceneObjectId, selectedObjectId, true);
		ImGui::PopID();
		if (nodeOpen)
		{
			auto childEntities = sceneObject->GetChildEntities();
			for (Ref<Entity> entity : childEntities)
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
		displaySceneObjectContextMenu(scene, sceneObjectId, selectedObjectId, true);
		ImGui::PopID();
	}
}

void displaySceneLight(Ref<Scene> scene, uint32_t sceneLightId, int32_t& selectedObjectId)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	Ref<LightObject> lightObject = ECSRegistry::GetInstance().GetEntity<LightObject>(sceneLightId);

	if (selectedObjectId == sceneLightId)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	ImGui::TreeNodeEx(lightObject->GetEntityName()->c_str(), nodeFlags);
	if (ImGui::IsItemClicked())
		selectedObjectId = lightObject->GetId();
	ImGui::PushID(sceneLightId);
	displaySceneLightContextMenu(scene, sceneLightId, selectedObjectId, true);
	ImGui::PopID();
}

void BuildSceneHierarchy(Ref<Scene> scene, int32_t& selectedSceneObjectId)
{
	ImGui::Begin("Scene Hierarchy", 0, ImGuiWindowFlags_NoCollapse);
	bool sceneOpen = ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen);
	displaySceneObjectContextMenu(scene, -1, selectedSceneObjectId, false);
	if(sceneOpen)
	{
		//Lights
		bool lightOpen = ImGui::TreeNodeEx("Lights", ImGuiTreeNodeFlags_DefaultOpen);
		displaySceneLightContextMenu(scene, -1, selectedSceneObjectId, false);
		if (lightOpen)
		{
			int32_t selectedObject = selectedSceneObjectId;
			for (uint32_t sceneLightId : scene->GetSceneLightIds())
				displaySceneLight(scene, sceneLightId, selectedObject);
			selectedSceneObjectId = selectedObject;
			ImGui::TreePop();
		}

		//SceneObjects
		int32_t selectedObject = selectedSceneObjectId;
		for (uint32_t sceneObjectId : scene->GetSceneObjectIds())
			displaySceneObject(scene, sceneObjectId, selectedObject);
		selectedSceneObjectId = selectedObject;
		ImGui::TreePop();
	}
	ImGui::End();
}