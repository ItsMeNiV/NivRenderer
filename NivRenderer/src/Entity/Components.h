#pragma once
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"

namespace NewComponents
{
    struct TagComponent
    {
        std::string name = "";
    };

    struct TransformComponent
    {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};
        glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
        glm::vec3 degRotation = {0.0f, 0.0f, 0.0f};
    };

    struct MaterialComponent
    {
        MaterialAsset* materialAsset = nullptr;
    };

    struct MeshComponent
    {
        std::string path = "";
        MeshAsset* meshAsset = nullptr;
    };

    // Mandatory Components for each Entity Type
    struct CameraComponent
    {
        Camera* cameraPtr = nullptr;
        bool dirtyFlag = true;
    };

    struct DirectionalLightComponent
    {
        glm::vec3 lightColor = {1.0f, 1.0f, 1.0f};
        glm::vec3 direction = {0.0f, -1.0f, 0.0f};
        bool dirtyFlag = true;
    };

    struct PointLightComponent
    {
        glm::vec3 lightColor = {1.0f, 1.0f, 1.0f};
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        uint32_t strength = 50;
        bool dirtyFlag = true;
    };

    struct SceneObjectComponent
    {
        std::string modelPath = "";
        bool dirtyFlag = true;
    };

    struct SkyboxComponent
    {
        std::string textureFolder = "";
        std::array<std::string, 6> texturePaths = {"","","","","",""};
        std::array<TextureAsset*, 6> textureAssets = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
        bool flipTextures = false;
        bool dirtyFlag = true;
    };
}
