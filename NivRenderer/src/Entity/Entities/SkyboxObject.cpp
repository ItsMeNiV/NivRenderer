#include "SkyboxObject.h"
#include <filesystem>

#include "Entity/Assets/AssetManager.h"

SkyboxObject::SkyboxObject(uint32_t id)
    : Entity(id, std::string("Skybox")), m_FlipTextures(false)
{}

void SkyboxObject::SetTexturePathsFromFolder()
{
    //Assumes files are named "WHATEVER_Right.WHATEVER", "WHATEVER_Left.WHATEVER", etc.
    //Automatically populates Texture paths in the following order: Right, Left, Top, Bottom, Front, Back

    if (m_TextureFolder.back() != '/')
        m_TextureFolder += '/';

    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(m_TextureFolder))
    {
        std::string filename = entry.path().filename().string();
        std::cmatch m;
        if (std::regex_match(filename.c_str(), m, std::regex("^.*_[R,r]ight\..*$")))
            m_TexturePaths[0] = entry.path().string();
        if (std::regex_match(filename.c_str(), m, std::regex("^.*_[L,l]eft\..*$")))
            m_TexturePaths[1] = entry.path().string();
        if (std::regex_match(filename.c_str(), m, std::regex("^.*_[T,t]op\..*$")))
            m_TexturePaths[2] = entry.path().string();
        if (std::regex_match(filename.c_str(), m, std::regex("^.*_[B,b]ottom\..*$")))
            m_TexturePaths[3] = entry.path().string();
        if (std::regex_match(filename.c_str(), m, std::regex("^.*_[F,f]ront\..*$")))
            m_TexturePaths[4] = entry.path().string();
        if (std::regex_match(filename.c_str(), m, std::regex("^.*_[B,b]ack\..*$")))
            m_TexturePaths[5] = entry.path().string();
    }

    bool allPathsSet = true;
    for (auto& path : GetTexturePaths())
    {
        allPathsSet &= !path.empty();
    }
    if (allPathsSet)
        LoadTextures();
}

void SkyboxObject::LoadTextures()
{
    uint8_t i = 0;
    for (auto& path : GetTexturePaths())
    {
        if (!path.empty())
            m_TextureAssets[i] = AssetManager::GetInstance().LoadTexture(path, m_FlipTextures);
        i++;
    }
}

bool SkyboxObject::HasAllTexturesSet()
{
    for (auto& path : GetTexturePaths())
    {
        if (path.empty())
            return false;
    }

    return true;
}

std::vector<std::pair<std::string, Property>> SkyboxObject::GetEntityProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;
    returnVector.push_back({"Texture folder", {PropertyType::PATH, &m_TextureFolder, [this]()
    {
        m_TextureFolder = std::regex_replace(m_TextureFolder, std::regex("\\\\"), "\/");
        SetTexturePathsFromFolder(); 
    }}});
    uint8_t i = 1;
    for (auto& path : m_TexturePaths)
    {
        std::string label = std::string("Texture path[") + std::to_string(i) + "]";
        returnVector.push_back({label.c_str(), {PropertyType::STRING, &m_TexturePaths[i-1], [this]() {}}});
        i++;
    }
    returnVector.push_back({"Reload Textures", {PropertyType::BUTTON, nullptr, [this]() { LoadTextures(); }}});
    returnVector.push_back({"Flip Textures", {PropertyType::BOOL, &m_FlipTextures, [this]() {}}});

    return returnVector;
}
