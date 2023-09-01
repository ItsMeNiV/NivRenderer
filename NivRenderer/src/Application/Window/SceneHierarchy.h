#pragma once
#include "imgui.h"
#include "Application/Util/Instrumentor.h"

inline void displaySceneObjectContextMenu(const Ref<Scene>& scene, const uint32_t sceneObjectId, int32_t& selectedObjectId, const bool allowDelete)
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

		//Object is base "Scene" in Hierarchy
        if (!allowDelete && !scene->HasSkybox())
        {
            if (ImGui::MenuItem("Add Skybox"))
                scene->AddSkybox();
        }

		ImGui::EndPopup();
	}
}

inline void displaySceneLightContextMenu(const Ref<Scene> &scene, const uint32_t& sceneObjectId, int32_t& selectedObjectId, const bool allowDelete)
{
	if (ImGui::BeginPopupContextItem("Context Menu"))
	{
        if (!scene->HasDirectionalLight())
        {
            if (ImGui::MenuItem("Add Directional Light"))
            {
                scene->AddDirectionalLight();
            }   
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

inline void displaySceneObject(const Ref<Scene> &scene, const uint32_t& sceneObjectId, int32_t& selectedObjectId)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
	Ref<SceneObject> sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);

	if (selectedObjectId == sceneObjectId)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	if (!sceneObject->GetChildEntities().empty())
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

inline void displaySceneLight(const Ref<Scene> &scene, const uint32_t& sceneLightId, int32_t& selectedObjectId)
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

inline void BuildSceneHierarchy(const Ref<Scene> &scene, int32_t& selectedSceneObjectId)
{
    ImGuiTreeNodeFlags sceneNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
    if (selectedSceneObjectId == scene->GetId())
        sceneNodeFlags |= ImGuiTreeNodeFlags_Selected;

	ImGui::Begin("Active Scene", 0, ImGuiWindowFlags_NoCollapse);
    if(ImGui::BeginTabBar("Active Scene TabBar"))
    {
        if(ImGui::BeginTabItem("Scene Hierarchy"))
        {
            bool sceneOpen = ImGui::TreeNodeEx("Scene", sceneNodeFlags);
            if (ImGui::IsItemClicked())
                selectedSceneObjectId = scene->GetId();
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

		        //Skybox
                if (scene->HasSkybox())
                {
                    uint32_t skyboxId = scene->GetSkyboxObjectId();
                    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                    Ref<SkyboxObject> skyboxObject = ECSRegistry::GetInstance().GetEntity<SkyboxObject>(skyboxId);

                    if (selectedSceneObjectId == skyboxId)
                        nodeFlags |= ImGuiTreeNodeFlags_Selected;

                    ImGui::TreeNodeEx(skyboxObject->GetEntityName()->c_str(), nodeFlags);
                    if (ImGui::IsItemClicked())
                        selectedSceneObjectId = skyboxId;
                    ImGui::PushID(skyboxId);
                    if (ImGui::BeginPopupContextItem("Context Menu"))
                    {
                        if (ImGui::MenuItem("Delete"))
                        {
                            scene->RemoveSkyboxObject();
                            if (skyboxId == selectedSceneObjectId)
                                selectedSceneObjectId = -1;
                        }

                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                }

		        //SceneObjects
		        int32_t selectedObject = selectedSceneObjectId;
		        for (uint32_t sceneObjectId : scene->GetSceneObjectIds())
			        displaySceneObject(scene, sceneObjectId, selectedObject);
		        selectedSceneObjectId = selectedObject;
		        ImGui::TreePop();
	        }
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Performance"))
        {
            ImGui::SeparatorText("Performance");
            for (auto& it : Instrumentor::GetInstance().GetTimings())
            {
                ImGui::Text(std::format("{}: {}microseconds", it.first, std::to_string(it.second)).c_str());
            }
            ImGui::EndTabItem();   
        }
        ImGui::EndTabBar();
    }
	ImGui::End();
}