#pragma once
#include "Base.h"

class IdManager
{
public:
    IdManager(IdManager const&) = delete;
    void operator=(IdManager const&) = delete;

    static IdManager& GetInstance()
    {
        static IdManager instance;

        return instance;
    }

    const uint32_t CreateNewId() { return m_NextId++; }

private:
    IdManager() : m_NextId(0) {}

    uint32_t m_NextId;
};