#pragma once
#include "Base.h"

class Proxy
{
public:
    Proxy(uint32_t id) : m_Id(id) {}

private:
    uint32_t m_Id;
};