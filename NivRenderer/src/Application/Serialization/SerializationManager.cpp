#include "SerializationManager.h"
#include "Entity/ECSRegistry.h"
#include "portable-file-dialogs.h"

void SerializationManager::SaveSceneToFile(Scene* const scene)
{
    auto destination = pfd::save_file("Save Scene", ".", {"NivRenderer Project", "*.nivproj"}).result();
    if (!destination.empty())
    {
        const std::string fileEnding =
            destination.find_last_of('.') == std::string::npos ? "" : destination.substr(destination.find_last_of('.'), destination.size());
        if (fileEnding != ".nivproj")
            destination += ".nivproj";
        std::ofstream oStream(destination);
        const nlohmann::ordered_json sceneJson = scene->SerializeObject();
        oStream << sceneJson;
        oStream.close();   
    }
}

Scene* SerializationManager::LoadSceneFromFile()
{
    const auto paths = pfd::open_file("Load Scene", ".", {"NivRenderer Project", "*.nivproj"}).result();
    if (!paths.empty())
    {
        std::ifstream iStream(paths[0]);
        nlohmann::json fileJson;

        iStream >> fileJson;

        ECSRegistry::GetInstance().ClearRegistry();
        Scene* scene = new Scene();
        scene->DeSerializeObject(fileJson);
        return scene;
    }

    return nullptr;
}
