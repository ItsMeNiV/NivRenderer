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
    const auto user_data = static_cast<InputTextCallback_UserData*>(data->UserData);
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		std::string* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = const_cast<char*>(str->c_str());
	}
	else if (user_data->ChainCallback)
	{
		// Forward to user callback, if any
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

inline void BuildProperties(const int32_t& selectedSceneObject, const Ref<Scene>& scene)
{
	ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse);

	if (selectedSceneObject == -1)
	{
		ImGui::End();
		return;
	}

    if (const Ref<SceneObject> sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(selectedSceneObject))
	{
		ImGui::SeparatorText(sceneObject->GetEntityName()->c_str());

		//Model-Path
        ImGui::InputText("Model Path", sceneObject->GetModelPath(), 0, InputTextCallback, sceneObject->GetModelPath());
        ImGui::PushID("Reload##Model");
        if (ImGui::Button("Reload") && !sceneObject->GetModelPath()->empty())
        {
            *sceneObject->GetModelPath() = std::regex_replace(*sceneObject->GetModelPath(), std::regex("\\\\"), "\/");
            sceneObject->LoadMeshAndMaterial();
            sceneObject->SetDirtyFlag(true);
        }
        ImGui::PopID();
        ImGui::Spacing();

        const std::vector<Ref<Component>> components = ECSRegistry::GetInstance().GetAllComponents(selectedSceneObject);
		for (const auto& component : components)
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
                    case NivRenderer::PropertyType::FLOAT2:
                        wasEdited = ImGui::InputFloat2(label, static_cast<float*>(it.second.valuePtr));
                        ImGui::Spacing();
                        break;
					case NivRenderer::PropertyType::FLOAT3:
                        wasEdited = ImGui::InputFloat3(label, static_cast<float*>(it.second.valuePtr));
                        ImGui::Spacing();
						break;
                    case NivRenderer::PropertyType::COLOR:
                        ImGui::ColorEdit3(label, static_cast<float*>(it.second.valuePtr));
                        ImGui::Spacing();
                        break;
					case NivRenderer::PropertyType::INT:
                        wasEdited = ImGui::InputInt(label, static_cast<int*>(it.second.valuePtr));
                        ImGui::Spacing();
						break;
                    case NivRenderer::PropertyType::INT2:
                        ImGui::InputInt2(label, static_cast<int*>(it.second.valuePtr));
                        ImGui::Spacing();
                        break;
                    case NivRenderer::PropertyType::SLIDER:
                        wasEdited = ImGui::SliderInt(label, static_cast<int*>(it.second.valuePtr), 1, 100);
                        ImGui::Spacing();
                        break;
					case NivRenderer::PropertyType::PATH:
					{
                            auto* inputString = static_cast<std::string*>(it.second.valuePtr);
                            ImGui::InputText(label, inputString, 0, InputTextCallback, (void*)inputString);
                            ImGui::PushID((std::string("Reload") + it.first).c_str());
                            if (ImGui::Button("Reload"))
                            {
                                *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                                it.second.callback();
                                wasEdited = true;
                            }
                            ImGui::PopID();
                            ImGui::Spacing();
                            break;   
					}
                    case NivRenderer::PropertyType::STRING:
					{
                            auto* inputString = static_cast<std::string*>(it.second.valuePtr);
                            ImGui::InputText(label, inputString, 0, InputTextCallback, (void*)inputString);
                            ImGui::Spacing();
                            break;   
					}
                    case NivRenderer::PropertyType::BUTTON:
                        ImGui::PushID((it.first + std::string("##") + std::string(component->GetName())).c_str());
                        if (ImGui::Button(it.first.c_str()))
                        {
                            it.second.callback();
                            wasEdited = true;
                        }
                        ImGui::PopID();
                        ImGui::Spacing();
                        break;
					case NivRenderer::PropertyType::BOOL:
                        wasEdited = ImGui::Checkbox(it.first.c_str(), static_cast<bool*>(it.second.valuePtr));
                        ImGui::Spacing();
                        break;
                    case NivRenderer::PropertyType::SEPARATORTEXT:
                        ImGui::SeparatorText(it.first.c_str());
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
        const Ref<Entity> selectedEntity = ECSRegistry::GetInstance().GetEntity<Entity>(selectedSceneObject);
        std::vector<std::pair<std::string, EntityProperty>> entityProperties;
        if (selectedSceneObject == scene->GetId())
            entityProperties = scene->GetEntityProperties();
        else
            entityProperties = selectedEntity->GetEntityProperties();

        for (auto& it : entityProperties)
        {
            bool wasEdited = false;
            const char* label = it.first.c_str();
            std::string entityName(selectedEntity ? selectedEntity->GetEntityName()->c_str() : "UNKNOWN");
            switch (it.second.type)
            {
            case NivRenderer::PropertyType::FLOAT:
                wasEdited = ImGui::InputFloat(label, static_cast<float*>(it.second.valuePtr));
                ImGui::Spacing();
                break;
            case NivRenderer::PropertyType::FLOAT2:
                wasEdited = ImGui::InputFloat2(label, static_cast<float*>(it.second.valuePtr));
                ImGui::Spacing();
                break;
            case NivRenderer::PropertyType::FLOAT3:
                wasEdited = ImGui::InputFloat3(label, static_cast<float*>(it.second.valuePtr));
                ImGui::Spacing();
                break;
            case NivRenderer::PropertyType::COLOR:
                ImGui::ColorEdit3(label, static_cast<float*>(it.second.valuePtr));
                ImGui::Spacing();
                break;
            case NivRenderer::PropertyType::INT:
                wasEdited = ImGui::InputInt(label, static_cast<int*>(it.second.valuePtr));
                ImGui::Spacing();
                break;
            case NivRenderer::PropertyType::INT2:
                ImGui::InputInt2(label, static_cast<int*>(it.second.valuePtr));
                ImGui::Spacing();
                break;
            case NivRenderer::PropertyType::SLIDER:
                wasEdited = ImGui::SliderInt(label, static_cast<int*>(it.second.valuePtr), 1, 100);
                ImGui::Spacing();
                break;
            case NivRenderer::PropertyType::PATH:
                {
                    auto* inputString = static_cast<std::string*>(it.second.valuePtr);
                    ImGui::InputText(label, inputString, 0, InputTextCallback, (void*)inputString);
                    ImGui::PushID((std::string("Reload") + it.first).c_str());
                    if (ImGui::Button("Reload"))
                    {
                        *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                        it.second.callback();
                        wasEdited = true;
                    }
                    ImGui::PopID();
                    ImGui::Spacing();
                    break;
                }
            case NivRenderer::PropertyType::STRING:
                {
                    auto* inputString = static_cast<std::string*>(it.second.valuePtr);
                    ImGui::InputText(label, inputString, 0, InputTextCallback, (void*)inputString);
                    ImGui::Spacing();
                    break;
                }
            case NivRenderer::PropertyType::BUTTON:
                ImGui::PushID((it.first + std::string("##") + entityName).c_str());
                if (ImGui::Button(it.first.c_str()))
                {
                    it.second.callback();
                    wasEdited = true;
                }
                ImGui::PopID();
                ImGui::Spacing();
                break;
            case NivRenderer::PropertyType::BOOL:
                wasEdited = ImGui::Checkbox(it.first.c_str(), static_cast<bool*>(it.second.valuePtr));
                ImGui::Spacing();
                break;
            case NivRenderer::PropertyType::SEPARATORTEXT:
                ImGui::SeparatorText(it.first.c_str());
                ImGui::Spacing();
                break;
            }

            if (wasEdited && selectedEntity)
                selectedEntity->SetDirtyFlag(true);
        }
	}

	ImGui::End();
}