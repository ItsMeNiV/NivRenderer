#include "SerializationManager.h"

void SerializationManager::SaveSceneToFile(const std::string& path, const Ref<Scene>& scene)
{
    std::ofstream oStream(path);
    const ordered_json sceneJson = scene->SerializeObject();
    oStream << std::setw(4) << sceneJson;
    oStream.close();
}
