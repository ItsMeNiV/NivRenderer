#pragma once
#include "Base.h"
#include "..\Entity\ECSRegistry.h"
#include "Scene.h"

class Project
{
public:
    Project(const std::string& projectPath);

    Scene* CreateScene();
    void RemoveScene(uint32_t sceneId);

    const std::string& GetPath() const { return m_Path; }
    void SetPath(const std::string& path) { m_Path = path; }
    Scene* GetActiveScene() const { return m_ActiveScene; }

    // Serialization
    nlohmann::ordered_json SerializeObject();
    void DeSerializeObject(nlohmann::json jsonObject);

private:
    std::vector<Scope<Scene>> m_Scenes;
    Scene* m_ActiveScene;

    std::string m_Path;
};
