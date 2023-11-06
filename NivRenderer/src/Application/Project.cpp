#include "Project.h"

#include "IdManager.h"
#include "Entity/ECSRegistry.h"
#include "Assets/AssetManager.h"

Project::Project(const std::string& projectPath) : m_Path(projectPath)
{
    IdManager::GetInstance().Reset();
    ECSRegistry::GetInstance().Reset();
    AssetManager::GetInstance().Reset();

    m_ActiveScene = CreateScene();
}

Scene* Project::CreateScene()
{
    Scope<Scene> newScene = CreateScope<Scene>();
    Scene* scenePtr = newScene.get();
    m_Scenes.push_back(std::move(newScene));
    return scenePtr;
}

void Project::RemoveScene(uint32_t sceneId)
{
    m_Scenes.erase(
        std::ranges::remove_if(m_Scenes, [sceneId](const Scope<Scene>& scene) { return scene->GetId() == sceneId; })
            .begin(),
        m_Scenes.end());
}

nlohmann::ordered_json Project::SerializeObject()
{
    using namespace nlohmann;
    ordered_json project;
    project["Path"] = m_Path;
    project["Scenes"] = json::array();
    project["ActiveSceneId"] = m_ActiveScene->GetId();
    for (uint32_t i = 0; i < m_Scenes.size(); i++)
    {
        project["Scenes"][i] = m_Scenes[i]->SerializeObject();
    }
    return project;
}

void Project::DeSerializeObject(nlohmann::json jsonObject)
{
    using namespace nlohmann;
    m_Path = jsonObject["Path"];
    json scenes = jsonObject["Scenes"];
    uint32_t i = 0;
    for (const json& sceneJson : scenes)
    {
        IdManager::GetInstance().SetNextId(sceneJson["Id"]);
        Scene* scene = CreateScene();
        scene->DeSerializeObject(sceneJson);

        if (jsonObject["ActiveSceneId"] == scene->GetId())
            m_ActiveScene = scene;

        i++;
    }
}
