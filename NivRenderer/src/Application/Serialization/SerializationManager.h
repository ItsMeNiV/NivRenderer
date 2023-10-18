#pragma once
#include "Base.h"
#include "Application/Project.h"

class SerializationManager
{
public:
    static void SaveProject(Project* const project, bool saveOnSetPath, bool temporarySave);
    static Project* LoadProject();
    static Project* LoadProject(const std::string& path);
};
