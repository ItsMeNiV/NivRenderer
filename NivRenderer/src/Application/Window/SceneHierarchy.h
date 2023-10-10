#pragma once
#include "imgui.h"
#include "Application/Util/Instrumentor.h"
#include "Application/NewScene.h"
#include "Entity/Assets/AssetManager.h"

inline void displaySceneObjectContextMenu(NewScene* scene, const uint32_t sceneObjectId, int32_t& selectedObjectId, const bool allowDelete)
{
	if (ImGui::BeginPopupContextItem("Context Menu"))
	{
		if (ImGui::MenuItem("Add SceneObject"))
		{
			scene->AddSceneObject(sceneObjectId);
		}
        if (ImGui::MenuItem("Add empty SceneObject"))
        {
            scene->AddEmptySceneObject(sceneObjectId);
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

inline void displaySceneLightContextMenu(NewScene* scene, const uint32_t& lightId, int32_t& selectedObjectId,
                                         const bool allowDelete, const bool isDirectionalLight)
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
            if (isDirectionalLight)
                scene->RemoveDirectionalLight();
            else
                scene->RemovePointLight(lightId);
			if (lightId == selectedObjectId)
				selectedObjectId = -1;
		}

		ImGui::EndPopup();
	}
}

inline void displayMaterialAssetContextMenu(NewScene* scene, const uint32_t sceneObjectId,
                                          int32_t& selectedObjectId, const bool allowDelete)
{
    if (ImGui::BeginPopupContextItem("Context Menu"))
    {
        if (!allowDelete && ImGui::MenuItem("Add Material"))
        {
            scene->AddMaterialAsset();
        }

        if (allowDelete && ImGui::MenuItem("Delete"))
        {
            scene->RemoveMaterialAsset(sceneObjectId);
            if (sceneObjectId == selectedObjectId)
                selectedObjectId = -1;
        }

        ImGui::EndPopup();
    }
}

inline void displaySceneObject(NewScene* scene, const SceneHierarchyElement& sceneHierarchyElement, int32_t& selectedObjectId)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
    const auto sceneObjectComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::SceneObjectComponent>(sceneHierarchyElement.entityId);
    const auto sceneObjectTagComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::TagComponent>(sceneHierarchyElement.entityId);

	if (selectedObjectId == sceneHierarchyElement.entityId)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	if (!sceneHierarchyElement.childIds.empty())
	{
        const bool nodeOpen = ImGui::TreeNodeEx(sceneObjectTagComponent->name.c_str(), nodeFlags);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            selectedObjectId = sceneHierarchyElement.entityId;
        ImGui::PushID(sceneHierarchyElement.entityId);
        displaySceneObjectContextMenu(scene, sceneHierarchyElement.entityId, selectedObjectId, true);
		ImGui::PopID();
		if (nodeOpen)
		{
            for (const auto childId : sceneHierarchyElement.childIds)
			{
                displaySceneObject(scene, *scene->GetSceneHierarchyElementById(childId), selectedObjectId);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(sceneObjectTagComponent->name.c_str(), nodeFlags);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            selectedObjectId = sceneHierarchyElement.entityId;
        ImGui::PushID(sceneHierarchyElement.entityId);
        displaySceneObjectContextMenu(scene, sceneHierarchyElement.entityId, selectedObjectId, true);
		ImGui::PopID();
	}
}

inline void displaySceneLight(NewScene* scene, const uint32_t& sceneLightId, int32_t& selectedObjectId, const bool isDirectionalLight)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    const auto tagComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::TagComponent>(sceneLightId);

	if (selectedObjectId == sceneLightId)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	ImGui::TreeNodeEx(tagComponent->name.c_str(), nodeFlags);
	if (ImGui::IsItemClicked())
        selectedObjectId = sceneLightId;
	ImGui::PushID(sceneLightId);
    displaySceneLightContextMenu(scene, sceneLightId, selectedObjectId, true, isDirectionalLight);
	ImGui::PopID();
}

inline void displayMaterialAsset(NewScene* scene, const uint32_t& sceneAssetId, int32_t& selectedObjectId)
{
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    const auto materialAsset = AssetManager::GetInstance().GetMaterial(sceneAssetId);

    if (selectedObjectId == sceneAssetId)
        nodeFlags |= ImGuiTreeNodeFlags_Selected;

    ImGui::TreeNodeEx(materialAsset->GetName().c_str(), nodeFlags);
    if (ImGui::IsItemClicked())
        selectedObjectId = materialAsset->GetId();
    ImGui::PushID(sceneAssetId);
    displayMaterialAssetContextMenu(scene, sceneAssetId, selectedObjectId, true);
    ImGui::PopID();
}

inline void BuildSceneHierarchy(NewScene* scene, int32_t& selectedSceneObjectId)
{
    ImGuiTreeNodeFlags sceneNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
    if (selectedSceneObjectId == scene->GetId())
        sceneNodeFlags |= ImGuiTreeNodeFlags_Selected;

	ImGui::Begin("Active Scene", 0, ImGuiWindowFlags_NoCollapse);
    if(ImGui::BeginTabBar("Active Scene TabBar"))
    {
        if(ImGui::BeginTabItem("Scene Hierarchy"))
        {
            const bool sceneOpen = ImGui::TreeNodeEx("Scene", sceneNodeFlags);
            if (ImGui::IsItemClicked())
                selectedSceneObjectId = scene->GetId();

	        displaySceneObjectContextMenu(scene, -1, selectedSceneObjectId, false);
	        if(sceneOpen)
	        {
		        //Lights
                const bool lightOpen = ImGui::TreeNodeEx("Lights", ImGuiTreeNodeFlags_DefaultOpen);
		        displaySceneLightContextMenu(scene, -1, selectedSceneObjectId, false, false);
		        if (lightOpen)
		        {
			        int32_t selectedObject = selectedSceneObjectId;
                    if (scene->HasDirectionalLight())
                        displaySceneLight(scene, scene->GetDirectionalLightId(), selectedObject, true);
			        for (uint32_t sceneLightId : scene->GetPointLightIds())
				        displaySceneLight(scene, sceneLightId, selectedObject, false);
			        selectedSceneObjectId = selectedObject;
			        ImGui::TreePop();
		        }

		        //Skybox
                if (scene->HasSkybox())
                {
                    const uint32_t skyboxId = scene->GetSkyboxObjectId();
                    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                    const auto skyboxComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::SkyboxComponent>(skyboxId);
                    const auto skyboxTagComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::TagComponent>(skyboxId);

                    if (selectedSceneObjectId == skyboxId)
                        nodeFlags |= ImGuiTreeNodeFlags_Selected;

                    ImGui::TreeNodeEx(skyboxTagComponent->name.c_str(), nodeFlags);
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
		        for (auto& sceneHierarchyElem : scene->GetSceneHierarchy())
		        {
                    if (sceneHierarchyElem.parentId == UINT32_MAX)
                        displaySceneObject(scene, sceneHierarchyElem, selectedObject);
		        }
		        selectedSceneObjectId = selectedObject;
		        ImGui::TreePop();
	        }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Assets"))
        {
            const bool assetsOpen = ImGui::TreeNodeEx("Materials", sceneNodeFlags);
            displayMaterialAssetContextMenu(scene, -1, selectedSceneObjectId, false);
            if (assetsOpen)
            {
                int32_t selectedObject = selectedSceneObjectId;
                for (uint32_t assetId : AssetManager::GetInstance().GetMaterialIds(false))
                    displayMaterialAsset(scene, assetId, selectedObject);
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