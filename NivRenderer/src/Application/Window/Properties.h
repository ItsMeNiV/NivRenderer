#pragma once
#include "Entity/ECSRegistry.h"
#include "Entity/Entities/SceneObject.h"
#include "Entity/Entities/LightObject.h"
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

void BuildProperties(int32_t& selectedSceneObject, const Ref<Scene>& scene)
{
	ImGui::Begin("Properties", 0, ImGuiWindowFlags_NoCollapse);

	if (selectedSceneObject == -1)
	{
		ImGui::End();
		return;
	}
    if (selectedSceneObject == scene->GetId())
    {
        ImGui::SeparatorText("General Scene Settings");
        ImGui::Checkbox("Visualize Lights", &scene->GetSceneSettings().visualizeLights);
        ImGui::InputInt2("Render resolution", glm::value_ptr(scene->GetSceneSettings().renderResolution));
        ImGui::InputInt("Sample count", (int32_t*)&scene->GetSceneSettings().sampleCount);
    }

	Ref<SceneObject> sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(selectedSceneObject);

	if (sceneObject)
	{
		ImGui::SeparatorText(sceneObject->GetEntityName()->c_str());

		//Model-Path
        ImGui::InputText("Model Path", sceneObject->GetModelPath(), 0, InputTextCallback, sceneObject->GetModelPath());
        ImGui::PushID("Reload##Model");
        if (ImGui::Button("Reload"))
        {
            sceneObject->LoadMeshAndMaterial();
            sceneObject->SetDirtyFlag(true);
        }
        ImGui::PopID();
        ImGui::Spacing();

		std::vector<Ref<Component>> components = ECSRegistry::GetInstance().GetAllComponents(selectedSceneObject);
		for (auto& component : components)
		{
			if (ImGui::CollapsingHeader(component->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (auto& it : component->GetComponentProperties())
				{
					bool wasEdited = false;
					const char* label = it.first.c_str();
					switch (it.second.type)
					{
					case NivRenderer::PropertyType::FLOAT:
						wasEdited = ImGui::InputFloat(label, static_cast<float*>(it.second.valuePtr));
                        ImGui::Spacing();
						break;
					case NivRenderer::PropertyType::FLOAT3:
                        wasEdited = ImGui::InputFloat3(label, static_cast<float*>(it.second.valuePtr));
                        ImGui::Spacing();
						break;
					case NivRenderer::PropertyType::INT:
                        wasEdited = ImGui::InputInt(label, static_cast<int*>(it.second.valuePtr));
                        ImGui::Spacing();
						break;
					case NivRenderer::PropertyType::PATH:
					{
                            std::string* inputString = static_cast<std::string*>(it.second.valuePtr);
                            ImGui::InputText(label, inputString, 0, InputTextCallback, (void*)inputString);
                            ImGui::PushID((std::string("Reload") + it.first).c_str());
                            if (ImGui::Button("Reload"))
                            {
                                it.second.callback();
                                wasEdited = true;
                            }
                            ImGui::PopID();
                            break;   
					}
					case NivRenderer::PropertyType::BOOL:
                        wasEdited = ImGui::Checkbox(it.first.c_str(), static_cast<bool*>(it.second.valuePtr));
                        ImGui::Spacing();
                        break;
					}

					if (wasEdited)
						sceneObject->SetDirtyFlag(true);
				}
			}
		}
	}
	else
	{
		Ref<DirectionalLightObject> directionalLightObject = ECSRegistry::GetInstance().GetEntity<DirectionalLightObject>(selectedSceneObject);
        Ref<PointLightObject> pointLightObject = ECSRegistry::GetInstance().GetEntity<PointLightObject>(selectedSceneObject);

		if (directionalLightObject)
		{
			bool wasEdited = false;
			wasEdited = ImGui::ColorEdit3("Light Color", glm::value_ptr(directionalLightObject->GetLightColor()));
			wasEdited |= ImGui::InputFloat3("Light Direction", glm::value_ptr(directionalLightObject->GetDirection()));

			if (wasEdited)
				directionalLightObject->SetDirtyFlag(true);
		}
        else if (pointLightObject)
        {
            bool wasEdited = false;
            wasEdited = ImGui::ColorEdit3("Light Color", glm::value_ptr(pointLightObject->GetLightColor()));
            wasEdited |= ImGui::InputFloat3("Light Position", glm::value_ptr(pointLightObject->GetPosition()));
            int32_t strengthInput = pointLightObject->GetStrength();
            wasEdited |= ImGui::SliderInt("Light Strength", &strengthInput, 1, 100);
            pointLightObject->SetStrength(std::min(std::max(strengthInput, 1), 100));

            if (wasEdited)
				pointLightObject->SetDirtyFlag(true);
        }
	}

	ImGui::End();
}