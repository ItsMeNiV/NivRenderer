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
    if (selectedSceneObject == scene->GetId())
    {
        ImGui::SeparatorText("General Scene Settings");
        ImGui::Checkbox("Visualize Lights", &scene->GetSceneSettings().visualizeLights);
        ImGui::Checkbox("Animate Directional Light", &scene->GetSceneSettings().animateDirectionalLight);
        ImGui::InputInt2("Render resolution", glm::value_ptr(scene->GetSceneSettings().renderResolution));
        ImGui::InputInt("Sample count", reinterpret_cast<int32_t*>(&scene->GetSceneSettings().sampleCount));
    }

    if (const Ref<SceneObject> sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(selectedSceneObject))
	{
		ImGui::SeparatorText(sceneObject->GetEntityName()->c_str());

		//Model-Path
        ImGui::InputText("Model Path", sceneObject->GetModelPath(), 0, InputTextCallback, sceneObject->GetModelPath());
        ImGui::PushID("Reload##Model");
        if (ImGui::Button("Reload"))
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
        const Ref<DirectionalLightObject> directionalLightObject = ECSRegistry::GetInstance().GetEntity<DirectionalLightObject>(selectedSceneObject);
        const Ref<PointLightObject> pointLightObject = ECSRegistry::GetInstance().GetEntity<PointLightObject>(selectedSceneObject);
        const Ref<SkyboxObject> skyboxObject = ECSRegistry::GetInstance().GetEntity<SkyboxObject>(selectedSceneObject);

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
        else if (skyboxObject)
        {
            ImGui::InputText("Texture folder", skyboxObject->GetTextureFolder(), 0, InputTextCallback, skyboxObject->GetTextureFolder());
            if (ImGui::Button("Populate Texture Paths"))
            {
                *skyboxObject->GetTextureFolder() =
                    std::regex_replace(*skyboxObject->GetTextureFolder(), std::regex("\\\\"), "\/");
                skyboxObject->SetTexturePathsFromFolder();
                skyboxObject->SetDirtyFlag(true);
            }

            uint8_t i = 1;
            for (auto& path : skyboxObject->GetTexturePaths())
            {
                std::string label = std::string("Texture path[") + std::to_string(i) + "]";
                ImGui::InputText(label.c_str(), &path, 0, InputTextCallback, &path);
                i++;
            }
            ImGui::PushID("Reload##Skybox");
            if (ImGui::Button("Reload"))
            {
                for (auto& path : skyboxObject->GetTexturePaths())
                    path = std::regex_replace(path, std::regex("\\\\"), "\/");
                skyboxObject->LoadTextures();
                skyboxObject->SetDirtyFlag(true);
            }
            ImGui::PopID();
            ImGui::Checkbox("Flip Textures", static_cast<bool*>(skyboxObject->GetFlipTextures()));
        }
	}

	ImGui::End();
}