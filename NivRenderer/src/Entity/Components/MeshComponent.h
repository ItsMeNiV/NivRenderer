#pragma once
#include "Base.h"
#include "Entity/Component.h"
#include "Entity/Assets/AssetManager.h"

class MeshComponent : public Component
{
public:
    MeshComponent(const uint32_t id);
	~MeshComponent() override = default;

    MeshAsset* GetMeshAsset() const;
    std::string& GetPath();
    void SetMeshAsset(MeshAsset* asset);

    std::vector<std::pair<std::string, Property>> GetComponentProperties() override;

    nlohmann::ordered_json SerializeObject() override;
    void DeSerializeObject(nlohmann::json jsonObject);

private:
	std::string m_Path;
    MeshAsset* m_MeshAsset;

    void reloadMesh();

};