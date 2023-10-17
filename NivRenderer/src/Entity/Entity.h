#pragma once
#include "entt/entt.hpp"

struct Entity
{
    uint32_t id;
    Scene* scenePtr = nullptr;
    entt::entity entityHandle{entt::null};

    Entity() = default;
    Entity(const Entity&) = delete;
    Entity(uint32_t Id, Scene* ScenePtr, entt::entity EntityHandle) :
        id(Id), scenePtr(ScenePtr), entityHandle(EntityHandle) {}
};