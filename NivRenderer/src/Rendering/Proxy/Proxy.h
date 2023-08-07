#pragma once
#include "Base.h"

class Proxy
{
public:
    Proxy(uint32_t id) : m_Id(id) {}
    virtual ~Proxy() {}

private:
    uint32_t m_Id;
};