#pragma once
#include "Entity/ECSRegistry.h"
#include "Entity/Entities/SceneObject.h"
#include "Entity/Component.h"

#include "imgui.h"

void BuildProperties(int32_t& selectedSceneObject)
{
	ImGui::Begin("Properties", 0, ImGuiWindowFlags_NoCollapse);

	if (selectedSceneObject == -1)
	{
		ImGui::End();
		return;
	}
	Ref<SceneObject> sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(selectedSceneObject);

	ImGui::SeparatorText(sceneObject->GetEntityName()->c_str());
	std::vector<Ref<Component>> components = ECSRegistry::GetInstance().GetAllComponents(selectedSceneObject);
	for (auto& component : components)
	{
		if (ImGui::CollapsingHeader(component->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto& it : component->GetComponentProperties())
			{
				switch (it.second.type)
				{
				case PropertyType::FLOAT:
					ImGui::InputFloat(it.first.c_str(), (float*)it.second.valuePtr);
					break;
				case PropertyType::FLOAT3:
					ImGui::InputFloat3(it.first.c_str(), (float*)it.second.valuePtr);
					break;
				case PropertyType::INT:
					ImGui::InputInt(it.first.c_str(), (int*)it.second.valuePtr);
					break;
				}
			}
		}
	}

	ImGui::End();
}