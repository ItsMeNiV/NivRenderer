#pragma once
#include "Base.h"

class Proxy
{
public:
    Proxy(uint32_t id) : m_Id(id), m_DirtyFlag(false) {}
    virtual ~Proxy() {}

    bool& GetDirtyFlag() { return m_DirtyFlag; }

private:
    uint32_t m_Id;
    bool m_DirtyFlag;
};