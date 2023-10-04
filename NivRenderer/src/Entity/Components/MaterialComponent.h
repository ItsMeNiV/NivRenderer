#pragma once
#include "Base.h"
#include "Entity/Component.h"
#include "Entity/Assets/MaterialAsset.h"

class MaterialComponent : public Component
{
public:
    MaterialComponent(const uint32_t id);
    ~MaterialComponent() override = default;

    std::vector<std::pair<std::string, Property>> GetComponentProperties() override;

    MaterialAsset* GetMaterialAsset() const { return m_MaterialAsset; }
    void SetMaterialAsset(MaterialAsset* asset) { m_MaterialAsset = asset; }

    nlohmann::ordered_json SerializeObject() override;
    void DeSerializeObject(nlohmann::json jsonObject);

private:
    MaterialAsset* m_MaterialAsset;
};
