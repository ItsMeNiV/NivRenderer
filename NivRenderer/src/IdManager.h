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

    uint32_t CreateNewId() { return m_NextId++; }
    uint32_t GetCurrentId() const { return m_NextId-1; }
    void SetNextId(const uint32_t id) { m_NextId = id; }

private:
    IdManager() : m_NextId(0) {}

    uint32_t m_NextId;
};