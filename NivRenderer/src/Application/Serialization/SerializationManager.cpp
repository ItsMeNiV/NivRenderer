#include "SerializationManager.h"

#include "Entity/ECSRegistry.h"

void SerializationManager::SaveSceneToFile(const std::string& path, Scene* const scene)
{
    std::ofstream oStream(path);
    const ordered_json sceneJson = scene->SerializeObject();
    oStream << std::setw(4) << sceneJson;
    oStream.close();
}

Scene* SerializationManager::LoadSceneFromFile(const std::string& path)
{
    std::ifstream iStream(path);
    json fileJson;

    iStream >> fileJson;

    ECSRegistry::GetInstance().ClearRegistry();
    Scene* scene = new Scene();
    scene->DeSerializeObject(fileJson);

    return scene;
}
