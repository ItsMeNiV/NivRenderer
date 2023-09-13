#include "SerializationManager.h"

#include "Entity/ECSRegistry.h"

void SerializationManager::SaveSceneToFile(const std::string& path, const Ref<Scene>& scene)
{
    std::ofstream oStream(path);
    const ordered_json sceneJson = scene->SerializeObject();
    oStream << std::setw(4) << sceneJson;
    oStream.close();
}

Ref<Scene> SerializationManager::LoadSceneFromFile(const std::string& path)
{
    std::ifstream iStream(path);
    json fileJson;

    iStream >> fileJson;

    ECSRegistry::GetInstance().ClearRegistry();
    Ref<Scene> scene = CreateRef<Scene>();
    scene->DeSerializeObject(fileJson);

    return scene;
}
