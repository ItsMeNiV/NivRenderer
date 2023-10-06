#pragma once
#include "Base.h"
#include "Application/Project.h"
#include "Application/Scene.h"

class SerializationManager
{
public:
    static void SaveProject(Project* const project, bool saveOnSetPath);
    static Project* LoadProject();
};
