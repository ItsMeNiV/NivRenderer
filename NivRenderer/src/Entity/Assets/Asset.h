#pragma once
#include "Base.h"
#include "Entity/PropertyType.h"

class Asset
{
public:
    virtual ~Asset() = default;
    Asset(uint32_t id) : m_Id(id) {}

    const uint32_t GetId() { return m_Id; }

    virtual std::vector<std::pair<std::string, NivRenderer::Property>> GetAssetProperties() = 0;

private:
    uint32_t m_Id;

};