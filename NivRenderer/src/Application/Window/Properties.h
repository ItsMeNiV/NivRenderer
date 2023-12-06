#pragma once
#include "Entity/ECSRegistry.h"
#include "Entity/Components.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "portable-file-dialogs.h"
#include "Assets/AssetManager.h"

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

inline void DrawTextureDropdown(TextureAsset** textureAsset)
{

    if (ImGui::BeginCombo("Texture", (*textureAsset) ? (*textureAsset)->path.c_str() : ""))
    {
        for (const uint32_t textureId : AssetManager::GetInstance().GetTextureIds(true))
        {
            const bool isSelected = (*textureAsset) ? (*textureAsset)->id == textureId : false;
            const auto currentTexture = AssetManager::GetInstance().GetTexture(textureId);
            if (ImGui::Selectable(currentTexture->path.c_str(), isSelected))
            {
                *textureAsset = currentTexture;
            }
        }
        ImGui::EndCombo();
    }
}

inline void BuildProperties(const int32_t& selectedSceneObject, Scene* scene)
{
	ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse);

	if (selectedSceneObject == -1)
	{
		ImGui::End();
		return;
	}

    if (selectedSceneObject == scene->GetId())
    {
        ImGui::Checkbox("Visualize Point Lights", &scene->GetSceneSettings().visualizeLights);
        ImGui::Checkbox("Animate Directional Light", &scene->GetSceneSettings().animateDirectionalLight);
        int msaaSamples = scene->GetSceneSettings().sampleCount;
        if (ImGui::InputInt("MSAA Sample Count", &msaaSamples))
        {
            scene->GetSceneSettings().sampleCount = std::max(1, msaaSamples);
        }
        ImGui::InputInt2("Render Resolution", glm::value_ptr(scene->GetSceneSettings().tempRenderResolution));
        ImGui::InputInt2("Render Resolution", glm::value_ptr(scene->GetSceneSettings().tempShadowmapResolution));
        if (ImGui::Button("Apply Resolution"))
        {
            scene->GetSceneSettings().renderResolution = scene->GetSceneSettings().tempRenderResolution;
            scene->GetSceneSettings().shadowmapResolution = scene->GetSceneSettings().tempShadowmapResolution;
        }
    }
    else
    {
        if (const auto materialAsset = AssetManager::GetInstance().GetMaterial(selectedSceneObject))
        {
            {
                auto* inputString = &materialAsset->name;
                ImGui::InputText("Name", inputString, 0, InputTextCallback, (void*)inputString);
                ImGui::Spacing();
            }

            // Diffuse
            {
                ImGui::SeparatorText("Diffuse");

                ImGui::PushID("TextureCombo##Diffuse");
                DrawTextureDropdown(&materialAsset->diffuseTextureAsset);
                ImGui::PopID();
                ImGui::PushID("Reload##Diffuse");
                if (ImGui::Button("Reload"))
                {
                    if (const auto textureAsset = materialAsset->diffuseTextureAsset)
                        materialAsset->diffuseTextureAsset = AssetManager::GetInstance().LoadTexture(
                            textureAsset->path, materialAsset->flipDiffuseTexture, textureAsset->loadOnlyOneChannel,
                            textureAsset->channelIndex);
                    // materialAsset->SetDirtyFlag(true); TODO
                }
                ImGui::SameLine();
                if (ImGui::Button("Open File"))
                {
                    const auto paths = pfd::open_file("Open File", ".").result();
                    if (!paths.empty())
                    {
                        auto path = paths[0];
                        path = std::regex_replace(path, std::regex("\\\\"), "\/");
                        materialAsset->diffuseTextureAsset = AssetManager::GetInstance().LoadTexture(path, false);
                        // materialAsset->SetDirtyFlag(true); TODO
                    }
                }
                ImGui::PopID();
                ImGui::Spacing();

                if (ImGui::Checkbox("Flip Diffuse Texture", &materialAsset->flipDiffuseTexture))
                {
                }
                // materialAsset->SetDirtyFlag(true); TODO
            }

            // Normal
            {
                ImGui::SeparatorText("Normal");

                ImGui::PushID("TextureCombo##Normal");
                DrawTextureDropdown(&materialAsset->normalTextureAsset);
                ImGui::PopID();
                ImGui::PushID("Reload##Normal");
                if (ImGui::Button("Reload"))
                {
                    if (const auto textureAsset = materialAsset->normalTextureAsset)
                        materialAsset->normalTextureAsset = AssetManager::GetInstance().LoadTexture(
                            textureAsset->path, materialAsset->flipNormalTexture, textureAsset->loadOnlyOneChannel,
                            textureAsset->channelIndex);
                    // materialAsset->SetDirtyFlag(true); TODO
                }
                ImGui::SameLine();
                if (ImGui::Button("Open File"))
                {
                    const auto paths = pfd::open_file("Open File", ".").result();
                    if (!paths.empty())
                    {
                        auto path = paths[0];
                        path = std::regex_replace(path, std::regex("\\\\"), "\/");
                        materialAsset->normalTextureAsset = AssetManager::GetInstance().LoadTexture(path, false);
                        // materialAsset->SetDirtyFlag(true); TODO
                    }
                }
                ImGui::PopID();
                ImGui::Spacing();

                if (ImGui::Checkbox("Flip Normal Texture", &materialAsset->flipNormalTexture))
                {
                }
                // materialAsset->SetDirtyFlag(true); TODO
            }

            // Metallic
            {
                ImGui::SeparatorText("Metallic");

                ImGui::PushID("TextureCombo##Metallic");
                DrawTextureDropdown(&materialAsset->metallicTextureAsset);
                ImGui::PopID();
                ImGui::PushID("Reload##Metallic");
                if (ImGui::Button("Reload"))
                {
                    if (const auto textureAsset = materialAsset->metallicTextureAsset)
                        materialAsset->metallicTextureAsset = AssetManager::GetInstance().LoadTexture(
                            textureAsset->path, materialAsset->flipMetallicTexture, textureAsset->loadOnlyOneChannel,
                            textureAsset->channelIndex);
                    // materialAsset->SetDirtyFlag(true); TODO
                }
                ImGui::SameLine();
                if (ImGui::Button("Open File"))
                {
                    const auto paths = pfd::open_file("Open File", ".").result();
                    if (!paths.empty())
                    {
                        auto path = paths[0];
                        path = std::regex_replace(path, std::regex("\\\\"), "\/");
                        materialAsset->metallicTextureAsset = AssetManager::GetInstance().LoadTexture(path, false);
                        // materialAsset->SetDirtyFlag(true); TODO
                    }
                }
                ImGui::PopID();
                ImGui::Spacing();

                if (ImGui::Checkbox("Flip Metallic Texture", &materialAsset->flipMetallicTexture))
                {
                }
                // materialAsset->SetDirtyFlag(true); TODO
            }

            // Roughness
            {
                ImGui::SeparatorText("Roughness");

                ImGui::PushID("TextureCombo##Roughness");
                DrawTextureDropdown(&materialAsset->roughnessTextureAsset);
                ImGui::PopID();
                ImGui::PushID("Reload##Roughness");
                if (ImGui::Button("Reload"))
                {
                    if (const auto textureAsset = materialAsset->roughnessTextureAsset)
                        materialAsset->roughnessTextureAsset = AssetManager::GetInstance().LoadTexture(
                            textureAsset->path, materialAsset->flipRoughnessTexture, textureAsset->loadOnlyOneChannel,
                            textureAsset->channelIndex);
                    // materialAsset->SetDirtyFlag(true); TODO
                }
                ImGui::SameLine();
                if (ImGui::Button("Open File"))
                {
                    const auto paths = pfd::open_file("Open File", ".").result();
                    if (!paths.empty())
                    {
                        auto path = paths[0];
                        path = std::regex_replace(path, std::regex("\\\\"), "\/");
                        materialAsset->roughnessTextureAsset = AssetManager::GetInstance().LoadTexture(path, false);
                        // materialAsset->SetDirtyFlag(true); TODO
                    }
                }
                ImGui::PopID();
                ImGui::Spacing();

                if (ImGui::Checkbox("Flip Roughness Texture", &materialAsset->flipRoughnessTexture))
                {
                }
                // materialAsset->SetDirtyFlag(true); TODO
            }

            // AO
            {
                ImGui::SeparatorText("AO");

                ImGui::PushID("TextureCombo##AO");
                DrawTextureDropdown(&materialAsset->aoTextureAsset);
                ImGui::PopID();
                ImGui::PushID("Reload##AO");
                if (ImGui::Button("Reload"))
                {
                    if (const auto textureAsset = materialAsset->aoTextureAsset)
                        materialAsset->aoTextureAsset = AssetManager::GetInstance().LoadTexture(
                            textureAsset->path, materialAsset->flipAOTexture, textureAsset->loadOnlyOneChannel,
                            textureAsset->channelIndex);
                    // materialAsset->SetDirtyFlag(true); TODO
                }
                ImGui::SameLine();
                if (ImGui::Button("Open File"))
                {
                    const auto paths = pfd::open_file("Open File", ".").result();
                    if (!paths.empty())
                    {
                        auto path = paths[0];
                        path = std::regex_replace(path, std::regex("\\\\"), "\/");
                        materialAsset->aoTextureAsset = AssetManager::GetInstance().LoadTexture(path, false);
                        // materialAsset->SetDirtyFlag(true); TODO
                    }
                }
                ImGui::PopID();
                ImGui::Spacing();

                if (ImGui::Checkbox("Flip AO Texture", &materialAsset->flipAOTexture))
                {
                }
                // materialAsset->SetDirtyFlag(true); TODO
            }

            // Emissive
            {
                ImGui::SeparatorText("Emissive");

                ImGui::PushID("TextureCombo##Emissive");
                DrawTextureDropdown(&materialAsset->emissiveTextureAsset);
                ImGui::PopID();
                ImGui::PushID("Reload##Emissive");
                if (ImGui::Button("Reload"))
                {
                    if (const auto textureAsset = materialAsset->emissiveTextureAsset)
                        materialAsset->emissiveTextureAsset = AssetManager::GetInstance().LoadTexture(
                            textureAsset->path, materialAsset->flipEmissiveTexture, textureAsset->loadOnlyOneChannel,
                            textureAsset->channelIndex);
                    // materialAsset->SetDirtyFlag(true); TODO
                }
                ImGui::SameLine();
                if (ImGui::Button("Open File"))
                {
                    const auto paths = pfd::open_file("Open File", ".").result();
                    if (!paths.empty())
                    {
                        auto path = paths[0];
                        path = std::regex_replace(path, std::regex("\\\\"), "\/");
                        materialAsset->emissiveTextureAsset = AssetManager::GetInstance().LoadTexture(path, false);
                        // materialAsset->SetDirtyFlag(true); TODO
                    }
                }
                ImGui::PopID();
                ImGui::Spacing();

                if (ImGui::Checkbox("Flip Emissive Texture", &materialAsset->flipEmissiveTexture))
                {
                }
                // materialAsset->SetDirtyFlag(true); TODO
            }
        }
        else
        {
            if (const auto tagComponent = ECSRegistry::GetInstance().GetComponent<TagComponent>(selectedSceneObject))
            {
                const auto inputString = &tagComponent->name;
                ImGui::InputText("Name", inputString, 0, InputTextCallback, (void*)inputString);
                ImGui::Spacing();
            }

            const auto sceneObjectComponent =
                ECSRegistry::GetInstance().GetComponent<SceneObjectComponent>(selectedSceneObject);
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
                    ECSRegistry::GetInstance().GetComponent<DirectionalLightComponent>(selectedSceneObject))
            {
                if (ImGui::ColorEdit3("Color", glm::value_ptr(directionalLightComponent->lightColor)))
                    directionalLightComponent->dirtyFlag = true;
                ImGui::Spacing();
                if (ImGui::InputFloat3("Direction", glm::value_ptr(directionalLightComponent->direction)))
                    directionalLightComponent->dirtyFlag = true;
                ImGui::Spacing();
            }

            if (const auto pointLightComponent =
                    ECSRegistry::GetInstance().GetComponent<PointLightComponent>(selectedSceneObject))
            {
                if (ImGui::ColorEdit3("Color", glm::value_ptr(pointLightComponent->lightColor)))
                    pointLightComponent->dirtyFlag = true;
                ImGui::Spacing();
                if (ImGui::InputFloat3("Position", glm::value_ptr(pointLightComponent->position)))
                    pointLightComponent->dirtyFlag = true;
                if (ImGui::SliderInt("Strength", &pointLightComponent->strength, 1, 100))
                    pointLightComponent->dirtyFlag = true;
                ImGui::Spacing();
            }

            if (const auto skyboxComponent =
                    ECSRegistry::GetInstance().GetComponent<SkyboxComponent>(selectedSceneObject))
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
                    if (ImGui::Button("Open Folder"))
                    {
                        const auto path = pfd::select_folder("Select Folder", ".").result();
                        if (!path.empty())
                        {
                            *inputString = path;
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
                    ImGui::InputText(("Path [" + std::to_string(i + 1) + "]").c_str(), inputString, 0,
                                     InputTextCallback, (void*)inputString);
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
                    ECSRegistry::GetInstance().GetComponent<CameraComponent>(selectedSceneObject))
            {
                bool isActiveCamera = scene->GetActiveCameraId() == selectedSceneObject;
                if (ImGui::Checkbox("Active", &isActiveCamera))
                {
                    if (isActiveCamera)
                        scene->SetActiveCamera(selectedSceneObject);
                }
            }

            if (const auto transformComponent =
                    ECSRegistry::GetInstance().GetComponent<TransformComponent>(selectedSceneObject))
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

            if (const auto meshComponent = ECSRegistry::GetInstance().GetComponent<MeshComponent>(selectedSceneObject))
            {
                if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    const auto inputString = &meshComponent->path;
                    ImGui::InputText("Path", inputString, 0, InputTextCallback, (void*)inputString);
                    ImGui::PushID((std::string("Reload") + std::to_string(selectedSceneObject)).c_str());
                    if (ImGui::Button("Reload"))
                    {
                        *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                        scene->LoadMesh(selectedSceneObject);
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
                            scene->LoadMesh(selectedSceneObject);
                            sceneObjectComponent->dirtyFlag = true;
                        }
                    }
                    ImGui::PopID();
                    ImGui::Spacing();
                }
            }

            if (const auto materialComponent =
                    ECSRegistry::GetInstance().GetComponent<MaterialComponent>(selectedSceneObject))
            {
                if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    auto* materialAsset = &materialComponent->materialAsset;
                    if (ImGui::BeginCombo("Material##Combo", (*materialAsset)->name.c_str()))
                    {
                        for (const uint32_t materialId : AssetManager::GetInstance().GetMaterialIds(true))
                        {
                            const bool isSelected = (*materialAsset)->id == materialId;
                            const auto currentMaterial = AssetManager::GetInstance().GetMaterial(materialId);
                            if (ImGui::Selectable(currentMaterial->name.c_str(), isSelected))
                            {
                                *materialAsset = currentMaterial;
                                sceneObjectComponent->dirtyFlag = true;
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
            }

            if (const auto customShaderComponent =
                    ECSRegistry::GetInstance().GetComponent<CustomShaderComponent>(selectedSceneObject))
            {
                if (ImGui::CollapsingHeader("Custom Shader", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    const auto inputString = &customShaderComponent->path;
                    ImGui::PushID((std::string("CustomShaderPath") + std::to_string(selectedSceneObject)).c_str());
                    ImGui::InputText("Path", inputString, 0, InputTextCallback, (void*)inputString);
                    ImGui::PopID();
                    ImGui::PushID((std::string("Recompile") + std::to_string(selectedSceneObject)).c_str());
                    if (ImGui::Button("Recompile"))
                    {
                        *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                        customShaderComponent->shaderAsset->shaderPtr->RecompileFromSource();
                        sceneObjectComponent->dirtyFlag = true;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Open File"))
                    {
                        const auto paths = pfd::open_file("Open File", ".", {"Shader Files", "*.glsl"}).result();
                        if (!paths.empty())
                        {
                            *inputString = paths[0];
                            *inputString = std::regex_replace(*inputString, std::regex("\\\\"), "\/");
                            AssetManager::GetInstance().LoadShader(customShaderComponent->path,
                                                                   ShaderType::VERTEX_AND_FRAGMENT);
                            sceneObjectComponent->dirtyFlag = true;
                        }
                    }
                    ImGui::PopID();
                    ImGui::Spacing();
                }
            }
        }
    }

	ImGui::End();
}