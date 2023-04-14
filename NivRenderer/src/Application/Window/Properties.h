#pragma once
#include "Entity/ECSRegistry.h"
#include "Entity/Entities/SceneObject.h"
#include "Entity/Component.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

struct InputTextCallback_UserData
{
	std::string* Str;
	ImGuiInputTextCallback  ChainCallback;
	void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		std::string* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}
	else if (user_data->ChainCallback)
	{
		// Forward to user callback, if any
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

void BuildProperties(int32_t& selectedSceneObject)
{
	ImGui::Begin("Properties", 0, ImGuiWindowFlags_NoCollapse);

	if (selectedSceneObject == -1)
	{
		ImGui::End();
		return;
	}
	Ref<SceneObject> sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(selectedSceneObject);

	if (sceneObject)
	{
		ImGui::SeparatorText(sceneObject->GetEntityName()->c_str());
		std::vector<Ref<Component>> components = ECSRegistry::GetInstance().GetAllComponents(selectedSceneObject);
		for (auto& component : components)
		{
			if (ImGui::CollapsingHeader(component->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (auto& it : component->GetComponentProperties())
				{
					const char* label = it.first.c_str();
					switch (it.second.type)
					{
					case PropertyType::FLOAT:
						ImGui::InputFloat(label, (float*)it.second.valuePtr);
						break;
					case PropertyType::FLOAT3:
						ImGui::InputFloat3(label, (float*)it.second.valuePtr);
						break;
					case PropertyType::INT:
						ImGui::InputInt(label, (int*)it.second.valuePtr);
						break;
					case PropertyType::PATH:
						std::string* inputString = (std::string*)it.second.valuePtr;
						ImGui::InputText(label, inputString, 0, InputTextCallback, (void*)inputString);
						ImGui::SameLine();
						if (ImGui::Button("Reload"))
						{
							it.second.callback();
						}
						break;
					}
				}
			}
		}
	}

	ImGui::End();
}