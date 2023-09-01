#pragma once
#include "Base.h"
#include "Entity/Entity.h"
#include "Entity/Assets/TextureAsset.h"

class SkyboxObject : public Entity
{
public:
    SkyboxObject(uint32_t id);

    void SetTexturePathsFromFolder();
    void LoadTextures();

    std::array<std::string, 6>& GetTexturePaths() { return m_TexturePaths; }
    std::string* GetTextureFolder() { return &m_TextureFolder; }
    bool* GetFlipTextures() { return &m_FlipTextures; }
    std::array<Ref<TextureAsset>, 6>& GetTextureAssets() { return m_TextureAssets; }
    bool HasAllTexturesSet();

    std::vector<std::pair<std::string, NivRenderer::Property>> GetEntityProperties() override;

private:
    std::string m_TextureFolder;
    std::array<std::string, 6> m_TexturePaths;
    std::array<Ref<TextureAsset>, 6> m_TextureAssets;
    bool m_FlipTextures;
};
