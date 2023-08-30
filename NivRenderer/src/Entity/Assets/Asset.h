#pragma once
#include "Base.h"

class Asset
{
public:
    Asset(uint32_t id) : m_Id(id) {}

    const uint32_t GetId() { return m_Id; }

private:
    uint32_t m_Id;

};