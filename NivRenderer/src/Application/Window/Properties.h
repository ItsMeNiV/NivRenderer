#pragma once
#include "Entity/NewECSRegistry.h"
#include "Entity/Components.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "portable-file-dialogs.h"

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

inline void BuildProperties(const int32_t& selectedSceneObject, NewScene* scene)
{
	ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse);

	if (selectedSceneObject == -1)
	{
		ImGui::End();
		return;
	}

    //TagComponent
    {
        const auto tagComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::TagComponent>(selectedSceneObject);
        const auto inputString = &tagComponent->name;
        ImGui::InputText("Name", inputString, 0, InputTextCallback, (void*)inputString);
        ImGui::Spacing();
    }

    const auto sceneObjectComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::SceneObjectComponent>(selectedSceneObject);
    if (sceneObjectComponent)
    {
        {
            const auto inputString = &sceneObjectComponent->modelPath;
            ImGui::InputText("Model Path", inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::PushID((std::string("Reload") + std::to_string(selectedSceneObject)).c_str());
            if (ImGui::Button("Reload"))
            {
                *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                scene->LoadModel(selectedSceneObject);
                sceneObjectComponent->dirtyFlag = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Open File"))
            {
                const auto paths = pfd::open_file("Open File", ".").result();
                if (!paths.empty())
                {
                    *inputString = paths[0];
                    *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                    scene->LoadModel(selectedSceneObject);
                    sceneObjectComponent->dirtyFlag = true;
                }
            }
            ImGui::PopID();
            ImGui::Spacing();
        }
    }

    if (const auto directionalLightComponent =
        NewECSRegistry::GetInstance().GetComponent<NewComponents::DirectionalLightComponent>(selectedSceneObject))
    {
        if (ImGui::ColorEdit3("Color", glm::value_ptr(directionalLightComponent->lightColor)))
            directionalLightComponent->dirtyFlag = true;
        ImGui::Spacing();
        if (ImGui::InputFloat3("Direction", glm::value_ptr(directionalLightComponent->direction)))
            directionalLightComponent->dirtyFlag = true;
        ImGui::Spacing();
    }

    if (const auto pointLightComponent =
        NewECSRegistry::GetInstance().GetComponent<NewComponents::PointLightComponent>(selectedSceneObject))
    {
        if (ImGui::ColorEdit3("Color", glm::value_ptr(pointLightComponent->lightColor)))
            pointLightComponent->dirtyFlag = true;
        ImGui::Spacing();
        if (ImGui::InputFloat3("Position", glm::value_ptr(pointLightComponent->position)))
            pointLightComponent->dirtyFlag = true;
        if (ImGui::InputInt("Strength", &pointLightComponent->strength))
            pointLightComponent->dirtyFlag = true;
        ImGui::Spacing();
    }

    if (const auto skyboxComponent =
        NewECSRegistry::GetInstance().GetComponent<NewComponents::SkyboxComponent>(selectedSceneObject))
    {
        {
            auto* inputString = &skyboxComponent->textureFolder;
            ImGui::InputText("Texture Folder", inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::PushID((std::string("Reload") + std::to_string(selectedSceneObject)).c_str());
            if (ImGui::Button("Reload"))
            {
                *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                scene->SetSkyboxTexturePathsFromFolder();
                skyboxComponent->dirtyFlag = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Open File"))
            {
                const auto paths = pfd::select_folder("Select Folder", ".").result();
                if (!paths.empty())
                {
                    *inputString = paths[0];
                    *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                    scene->SetSkyboxTexturePathsFromFolder();
                    skyboxComponent->dirtyFlag = true;
                }
            }
            ImGui::PopID();
            ImGui::Spacing();
        }
        for (uint32_t i = 0; i < 6; i++)
        {
            auto* inputString = &skyboxComponent->texturePaths[i];
            ImGui::InputText(("Path [" + std::to_string(i+1) + "]").c_str(), inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::Spacing();
        }

        {
            ImGui::PushID("Reload##Skybox");
            if (ImGui::Button("Reload Textures"))
            {
                scene->LoadSkyboxTextures();
                skyboxComponent->dirtyFlag = true;
            }
            ImGui::PopID();
            ImGui::Spacing();
        }
        if (ImGui::Checkbox("Flip Textures", &skyboxComponent->flipTextures))
            skyboxComponent->dirtyFlag = true;

    }

    if (const auto cameraComponent =
        NewECSRegistry::GetInstance().GetComponent<NewComponents::CameraComponent>(selectedSceneObject))
    {
        bool isActiveCamera = scene->GetActiveCameraId() == selectedSceneObject;
        if(ImGui::Checkbox("Active", &isActiveCamera))
        {
            if (isActiveCamera)
                scene->SetActiveCamera(selectedSceneObject);
        }

    }

    if (const auto transformComponent =
        NewECSRegistry::GetInstance().GetComponent<NewComponents::TransformComponent>(selectedSceneObject))
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::InputFloat3("Position", glm::value_ptr(transformComponent->position)))
                sceneObjectComponent->dirtyFlag = true;
            ImGui::Spacing();
            if (ImGui::InputFloat3("Scale", glm::value_ptr(transformComponent->scale)))
                sceneObjectComponent->dirtyFlag = true;
            ImGui::Spacing();
            transformComponent->degRotation = glm::degrees(transformComponent->rotation);
            if (ImGui::InputFloat3("Rotation", glm::value_ptr(transformComponent->degRotation)))
            {
                transformComponent->rotation = glm::radians(transformComponent->degRotation);
                sceneObjectComponent->dirtyFlag = true;
            }
            ImGui::Spacing();
        }
    }

    if (const auto meshComponent =
        NewECSRegistry::GetInstance().GetComponent<NewComponents::MeshComponent>(selectedSceneObject))
    {
        if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const auto inputString = &meshComponent->path;
            ImGui::InputText("Path", inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::PushID((std::string("Reload") + std::to_string(selectedSceneObject)).c_str());
            if (ImGui::Button("Reload"))
            {
                *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                scene->LoadModel(selectedSceneObject);
                sceneObjectComponent->dirtyFlag = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Open File"))
            {
                const auto paths = pfd::open_file("Open File", ".").result();
                if (!paths.empty())
                {
                    *inputString = paths[0];
                    *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                    scene->LoadModel(selectedSceneObject);
                    sceneObjectComponent->dirtyFlag = true;
                }
            }
            ImGui::PopID();
            ImGui::Spacing();
        }
    }

    if (const auto materialComponent =
        NewECSRegistry::GetInstance().GetComponent<NewComponents::MaterialComponent>(selectedSceneObject))
    {
        auto* materialAsset = &materialComponent->materialAsset;
        if (ImGui::BeginCombo("Material", (*materialAsset)->GetName().c_str()))
        {
            for (const uint32_t materialId : AssetManager::GetInstance().GetMaterialIds(true))
            {
                const bool isSelected = (*materialAsset)->GetId() == materialId;
                const auto currentMaterial = AssetManager::GetInstance().GetMaterial(materialId);
                if (ImGui::Selectable(currentMaterial->GetName().c_str(), isSelected))
                {
                    *materialAsset = currentMaterial;
                    sceneObjectComponent->dirtyFlag = true;
                }
            }
            ImGui::EndCombo();
        }
    }

    if(const auto materialAsset = AssetManager::GetInstance().GetMaterial(selectedSceneObject))
	{
        {
            auto* inputString = &materialAsset->GetName();
            ImGui::InputText("Name", inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::Spacing();
        }

        //Diffuse
        {
            ImGui::SeparatorText("Diffuse");

            auto* inputString = &materialAsset->GetDiffusePath();
            ImGui::InputText("Path", inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::PushID((std::string("Reload##Diffuse")).c_str());
            if (ImGui::Button("Reload"))
            {
                *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                materialAsset->ReloadDiffuseTexture();
                materialAsset->SetDirtyFlag(true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Open File"))
            {
                const auto paths = pfd::open_file("Open File", ".").result();
                if (!paths.empty())
                {
                    *inputString = paths[0];
                    *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                    materialAsset->ReloadDiffuseTexture();
                    materialAsset->SetDirtyFlag(true);
                }
            }
            ImGui::PopID();
            ImGui::Spacing();

            if (ImGui::Checkbox("Flip Diffuse Texture", &materialAsset->GetFlipDiffuseTexture()))
                materialAsset->SetDirtyFlag(true);
        }

        //Normal
        {
            ImGui::SeparatorText("Normal");

            auto* inputString = &materialAsset->GetNormalPath();
            ImGui::InputText("Path", inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::PushID((std::string("Reload##Normal")).c_str());
            if (ImGui::Button("Reload"))
            {
                *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                materialAsset->ReloadNormalTexture();
                materialAsset->SetDirtyFlag(true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Open File"))
            {
                const auto paths = pfd::open_file("Open File", ".").result();
                if (!paths.empty())
                {
                    *inputString = paths[0];
                    *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                    materialAsset->ReloadNormalTexture();
                    materialAsset->SetDirtyFlag(true);
                }
            }
            ImGui::PopID();
            ImGui::Spacing();

            if (ImGui::Checkbox("Flip Normal Texture", &materialAsset->GetFlipNormalTexture()))
                materialAsset->SetDirtyFlag(true);
        }

        //Metallic
        {
            ImGui::SeparatorText("Metallic");

            auto* inputString = &materialAsset->GetMetallicPath();
            ImGui::InputText("Path", inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::PushID((std::string("Reload##Metallic")).c_str());
            if (ImGui::Button("Reload"))
            {
                *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                materialAsset->ReloadMetallicTexture();
                materialAsset->SetDirtyFlag(true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Open File"))
            {
                const auto paths = pfd::open_file("Open File", ".").result();
                if (!paths.empty())
                {
                    *inputString = paths[0];
                    *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                    materialAsset->ReloadMetallicTexture();
                    materialAsset->SetDirtyFlag(true);
                }
            }
            ImGui::PopID();
            ImGui::Spacing();

            if (ImGui::Checkbox("Flip Metallic Texture", &materialAsset->GetFlipMetallicTexture()))
                materialAsset->SetDirtyFlag(true);
        }

        //Roughness
        {
            ImGui::SeparatorText("Roughness");

            auto* inputString = &materialAsset->GetRoughnessPath();
            ImGui::InputText("Path", inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::PushID((std::string("Reload##Roughness")).c_str());
            if (ImGui::Button("Reload"))
            {
                *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                materialAsset->ReloadRoughnessTexture();
                materialAsset->SetDirtyFlag(true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Open File"))
            {
                const auto paths = pfd::open_file("Open File", ".").result();
                if (!paths.empty())
                {
                    *inputString = paths[0];
                    *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                    materialAsset->ReloadRoughnessTexture();
                    materialAsset->SetDirtyFlag(true);
                }
            }
            ImGui::PopID();
            ImGui::Spacing();

            if (ImGui::Checkbox("Flip Roughness Texture", &materialAsset->GetFlipRoughnessTexture()))
                materialAsset->SetDirtyFlag(true);
        }

        //AO
        {
            ImGui::SeparatorText("AO");

            auto* inputString = &materialAsset->GetAOPath();
            ImGui::InputText("Path", inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::PushID((std::string("Reload##AO")).c_str());
            if (ImGui::Button("Reload"))
            {
                *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                materialAsset->ReloadAOTexture();
                materialAsset->SetDirtyFlag(true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Open File"))
            {
                const auto paths = pfd::open_file("Open File", ".").result();
                if (!paths.empty())
                {
                    *inputString = paths[0];
                    *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                    materialAsset->ReloadAOTexture();
                    materialAsset->SetDirtyFlag(true);
                }
            }
            ImGui::PopID();
            ImGui::Spacing();

            if (ImGui::Checkbox("Flip AO Texture", &materialAsset->GetFlipAOTexture()))
                materialAsset->SetDirtyFlag(true);
        }

        //Emissive
        {
            ImGui::SeparatorText("Emissive");

            auto* inputString = &materialAsset->GetEmissivePath();
            ImGui::InputText("Path", inputString, 0, InputTextCallback, (void*)inputString);
            ImGui::PushID((std::string("Reload##Emissive")).c_str());
            if (ImGui::Button("Reload"))
            {
                *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                materialAsset->ReloadEmissiveTexture();
                materialAsset->SetDirtyFlag(true);
            }
            ImGui::SameLine();
            if (ImGui::Button("Open File"))
            {
                const auto paths = pfd::open_file("Open File", ".").result();
                if (!paths.empty())
                {
                    *inputString = paths[0];
                    *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                    materialAsset->ReloadEmissiveTexture();
                    materialAsset->SetDirtyFlag(true);
                }
            }
            ImGui::PopID();
            ImGui::Spacing();

            if (ImGui::Checkbox("Flip Emissive Texture", &materialAsset->GetFlipEmissiveTexture()))
                materialAsset->SetDirtyFlag(true);
        }
	}

	ImGui::End();
}