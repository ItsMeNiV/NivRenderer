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

    void Reset() { m_NextId = 10; }

    uint32_t CreateNewId()
    {
        const uint32_t idToReturn = m_TempNextId != -1 ? m_TempNextId : m_NextId;
        if (idToReturn == m_TempNextId)
            m_TempNextId = -1;
        else
            m_NextId++;

        return idToReturn;
    }
    uint32_t GetCurrentId() const { return m_NextId-1; }
    void SetNextId(const uint32_t id) { m_TempNextId = id; }

private:
    IdManager() : m_NextId(10), m_TempNextId(-1) {} // First 10 Ids are reserved for internal use

    uint32_t m_NextId;
    int32_t m_TempNextId;
};