#pragma once
#include "Base.h"
#include "Entity/Component.h"
#include "Entity/Assets/AssetManager.h"

class MeshComponent : public Component
{
public:
    MeshComponent(const uint32_t id);
	~MeshComponent() = default;

    Ref<MeshAsset>& GetMeshAsset();
    std::string& GetPath();

    std::vector<std::pair<std::string, Property>> GetComponentProperties() override;

private:
	std::string m_Path;
    Ref<MeshAsset> m_MeshAsset;

    void reloadMesh();
};