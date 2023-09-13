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

    std::vector<Ref<SceneObjectProxy>> GetSceneObjectsToRender(const Ref<Scene>& scene);

private:
    std::unordered_map<uint32_t, Ref<Proxy>> m_Proxies;

    void updateSceneObjectProxy(const uint32_t sceneObjectId, const Ref<SceneObjectProxy>& parentProxy, std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap);
    void updateMaterialProxy(const uint32_t materialId, std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap);
    void updateCameraProxy(const uint32_t cameraId, std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap);
    void updateSkyboxProxy(const uint32_t skyboxId, std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap);
    void updateSceneLightProxies(const uint32_t sceneLightId, std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap);
    void setupMaterialProxy(const std::string& assetPath, Ref<TextureProxy>& textureProxy,
                            const Ref<TextureAsset>& textureAsset, const Ref<TextureAsset>& alternativeTextureAsset,
                            std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap);

    void addSceneObjectProxyAndChildrenToList(std::vector<Ref<SceneObjectProxy>>& list, const Ref<SceneObject>& sceneObject);
};