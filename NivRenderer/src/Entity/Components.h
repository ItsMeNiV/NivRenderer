#pragma once
#include "Assets/Assets.h"

// Optional Components
struct TransformComponent
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 degRotation = {0.0f, 0.0f, 0.0f};

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(glm::vec3 Position, glm::vec3 Scale, glm::vec3 Rotation, glm::vec3 DegRotation)
        : position(Position), scale(Scale), rotation(Rotation), degRotation(DegRotation) {}
};

struct MaterialComponent
{
    MaterialAsset* materialAsset = nullptr;

    MaterialComponent() = default;
    MaterialComponent(const MaterialComponent&) = default;
    MaterialComponent(MaterialAsset* MaterialAsset)
        : materialAsset(MaterialAsset) { }
};

struct MeshComponent
{
    std::string path = "";
    MeshAsset* meshAsset = nullptr;

    MeshComponent() = default;
    MeshComponent(const MeshComponent&) = default;
    MeshComponent(const std::string& Path, MeshAsset* MeshAsset)
        : path(Path), meshAsset(MeshAsset) {}
};

// Mandatory Components for each Entity Type
struct TagComponent
{
    std::string name = "";

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& Name)
        : name(Name) {}
};

struct CameraComponent
{
    Camera* cameraPtr = nullptr;
    bool dirtyFlag = true;

    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
    CameraComponent(Camera* CameraPtr)
        : cameraPtr(CameraPtr) {}
};

struct DirectionalLightComponent
{
    glm::vec3 lightColor = {1.0f, 1.0f, 1.0f};
    glm::vec3 direction = {0.0f, -1.0f, 0.0f};
    bool dirtyFlag = true;

    DirectionalLightComponent() = default;
    DirectionalLightComponent(const DirectionalLightComponent&) = default;
    DirectionalLightComponent(glm::vec3 LightColor, glm::vec3 Direction)
        : lightColor(LightColor), direction(Direction) {}
};

struct PointLightComponent
{
    glm::vec3 lightColor = {1.0f, 1.0f, 1.0f};
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    int32_t strength = 50;
    bool dirtyFlag = true;

    PointLightComponent() = default;
    PointLightComponent(const PointLightComponent&) = default;
    PointLightComponent(glm::vec3 LightColor, glm::vec3 Position, int32_t Strength)
        : lightColor(LightColor), position(Position), strength(Strength) {}
};

struct SceneObjectComponent
{
    std::string modelPath = "";
    bool dirtyFlag = true;

    SceneObjectComponent() = default;
    SceneObjectComponent(const SceneObjectComponent&) = default;
    SceneObjectComponent(const std::string& ModelPath)
        : modelPath(ModelPath) {}
};

struct SkyboxComponent
{
    std::string textureFolder = "";
    std::array<std::string, 6> texturePaths = {"","","","","",""};
    std::array<TextureAsset*, 6> textureAssets = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    bool flipTextures = false;
    bool dirtyFlag = true;

    SkyboxComponent() = default;
    SkyboxComponent(const SkyboxComponent&) = default;
    SkyboxComponent(const std::string& TextureFolder, bool FlipTextures)
        : textureFolder(TextureFolder), flipTextures(FlipTextures) {}
};

struct CustomShaderComponent
{
    std::string path = "";
    ShaderAsset* shaderAsset = nullptr;

    CustomShaderComponent() = default;
    CustomShaderComponent(const CustomShaderComponent&) = default;
    CustomShaderComponent(const std::string& Path, ShaderAsset* ShaderAsset)
        : path(Path), shaderAsset(ShaderAsset) {}
};
