#pragma once
#include "Base.h"
#include "Entity/Component.h"
#include "Entity/Assets/AssetManager.h"

class MeshComponent : public Component
{
public:
    MeshComponent();
	~MeshComponent() = default;

    Ref<MeshAsset>& GetMeshAsset();
    std::string& GetPath();

    std::unordered_map<std::string, ComponentProperty> GetComponentProperties() override;

private:
	std::string m_Path;
    Ref<MeshAsset> m_MeshAsset;

    void reloadMesh();
};