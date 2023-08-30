#pragma once
#include "Base.h"
#include "Application/Scene.h"
#include "Rendering/Proxy/Proxy.h"
#include "Rendering/Proxy/SceneObjectProxy.h"
#include "Rendering/Proxy/LightProxy.h"
#include "Rendering/Proxy/CameraProxy.h"
#include "Rendering/Proxy/SkyboxProxy.h"
#include "Rendering/Proxy/MeshProxy.h"
#include "Rendering/Proxy/TextureProxy.h"

class ProxyManager
{
public:
    ProxyManager();

    void UpdateProxies(const Ref<Scene>& scene);
    Ref<Proxy> GetProxy(const uint32_t id);

private:
    std::unordered_map<uint32_t, Ref<Proxy>> m_Proxies;

    void setupMaterialProxy(const std::string& assetPath, Ref<TextureProxy>& textureProxy,
                            const Ref<TextureAsset>& textureAsset, const Ref<TextureAsset>& alternativeTextureAsset);
};