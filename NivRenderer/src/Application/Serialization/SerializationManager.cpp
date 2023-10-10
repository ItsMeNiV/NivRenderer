#include "SerializationManager.h"
#include "portable-file-dialogs.h"

void SerializationManager::SaveProject(Project* const project, bool saveOnSetPath)
{
    std::string savePath = project->GetPath();
    if (savePath.empty() || !saveOnSetPath)
    {
        savePath = pfd::save_file("Save Scene", ".", {"NivRenderer Project", "*.nivproj"}).result();
        project->SetPath(savePath);
    }

    const std::string fileEnding = savePath.find_last_of('.') == std::string::npos
        ? ""
        : savePath.substr(savePath.find_last_of('.'), savePath.size());
    if (fileEnding != ".nivproj")
        savePath += ".nivproj";
    std::ofstream oStream(savePath);
    const nlohmann::ordered_json projectJson = project->SerializeObject();
    oStream << projectJson;
    oStream.close();
}

Project* SerializationManager::LoadProject()
{
    const auto paths = pfd::open_file("Load Project", ".", {"NivRenderer Project", "*.nivproj"}).result();
    if (!paths.empty())
    {
        std::ifstream iStream(paths[0]);
        nlohmann::json fileJson;

        iStream >> fileJson;

        const auto project = new Project(paths[0]);
        project->DeSerializeObject(fileJson);
        return project;
    }

    return nullptr;
}
