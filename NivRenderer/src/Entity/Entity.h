#pragma once
#include "..\Application\Scene.h"
#include "entt/entt.hpp"

struct Entity
{
    uint32_t id;
    Scene* scenePtr = nullptr;
    entt::entity entityHandle{entt::null};
};