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

    Ref<MaterialAsset>& GetMaterialAsset() { return m_MaterialAsset; }

    ordered_json SerializeObject() override;

private:
    Ref<MaterialAsset> m_MaterialAsset;
};
