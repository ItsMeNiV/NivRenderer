#pragma once
#include "Base.h"
#include "Application/Scene.h"

class SerializationManager
{
public:
    static void SaveSceneToFile(Scene* const scene);
    static Scene* LoadSceneFromFile();
};
