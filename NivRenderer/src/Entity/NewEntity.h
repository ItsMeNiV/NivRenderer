#pragma once
#include "Application/NewScene.h"
#include "entt/entt.hpp"

struct NewEntity
{
    uint32_t id;
    NewScene* scenePtr = nullptr;
    entt::entity entityHandle{entt::null};
};