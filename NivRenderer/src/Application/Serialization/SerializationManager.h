#pragma once
#include "Base.h"
#include "Application/Scene.h"

class SerializationManager
{
public:
    static void SaveSceneToFile(const std::string& path, Scene* const scene);
    static Scene* LoadSceneFromFile(const std::string& path);
};
