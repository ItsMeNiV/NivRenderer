#pragma once
#include "Base.h"

class NewECSRegistry
{
public:
    NewECSRegistry(class NewECSRegistry const&) = delete;
    void operator=(NewECSRegistry const&) = delete;

    static NewECSRegistry& GetInstance()
    {
        static NewECSRegistry instance;

        return instance;
    }

    void Reset();

private:
    NewECSRegistry() = default;

private:

};