#pragma once
#include "Base.h"
#include "Entity/Assets/MeshAsset.h"
#include "Entity/Assets/TextureAsset.h"

class AssetManager
{
public:
    AssetManager(AssetManager const &) = delete;
    void operator=(AssetManager const &) = delete;

    static AssetManager& GetInstance()
    {
        static AssetManager instance;
        return instance;
    }

    MeshAsset& LoadMesh(std::string& path)
    { 
        MeshAsset a;
        return a;
    }

    TextureAsset &LoadTexture(std::string &path)
    {
        TextureAsset a;
        return a;
    }

private:
    AssetManager() {}

    std::unordered_map<std::string, MeshAsset> m_LoadedMeshAssets;
    std::unordered_map<std::string, TextureAsset> m_LoadedTextureAssets;

};