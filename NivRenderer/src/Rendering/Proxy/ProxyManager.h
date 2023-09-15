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

    void UpdateProxies(const Scene* const scene);
    Proxy* GetProxy(const uint32_t id);

    std::vector<SceneObjectProxy*> GetSceneObjectsToRender(const Scene* const scene);

private:
    std::unordered_map<uint32_t, Scope<Proxy>> m_Proxies;

    void updateSceneObjectProxy(const uint32_t sceneObjectId, SceneObjectProxy* const parentProxy);
    void updateMaterialProxy(const uint32_t materialId);
    void updateCameraProxy(const uint32_t cameraId);
    void updateSkyboxProxy(const uint32_t skyboxId);
    void updateSceneLightProxies(const uint32_t sceneLightId);
    void setupMaterialProxy(const std::string& assetPath, TextureProxy** const textureProxy,
                            TextureAsset* const textureAsset, TextureAsset* const alternativeTextureAsset);

    void addSceneObjectProxyAndChildrenToList(std::vector<SceneObjectProxy*>& list,
                                              const SceneObject* const sceneObject);
};