#pragma once
#include "Base.h"
#include "Entity/NewECSRegistry.h"
#include "Application/NewScene.h"

class Project
{
public:
    Project(const std::string& projectPath);

    NewScene* CreateScene();
    void RemoveScene(uint32_t sceneId);

    const std::string& GetPath() const { return m_Path; }
    void SetPath(const std::string& path) { m_Path = path; }
    NewScene* GetActiveScene() const { return m_ActiveScene; }

    // Serialization
    nlohmann::ordered_json SerializeObject();
    void DeSerializeObject(nlohmann::json jsonObject);

private:
    std::vector<Scope<NewScene>> m_Scenes;
    NewScene* m_ActiveScene;

    std::string m_Path;
};
