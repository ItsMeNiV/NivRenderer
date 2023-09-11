#pragma once
#include "Base.h"
#include "Application/Scene.h"

class SerializationManager
{
public:
    static void SaveSceneToFile(const std::string& path, const Ref<Scene>& scene);
};
