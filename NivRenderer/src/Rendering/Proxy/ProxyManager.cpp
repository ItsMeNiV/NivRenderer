#include "ProxyManager.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"

ProxyManager::ProxyManager() = default;

void ProxyManager::UpdateProxies(const Ref<Scene>& scene)
{
    std::unordered_map<uint32_t, Ref<Proxy>> newProxyMap;

    updateCameraProxy(scene->GetCameraId(), newProxyMap);

    if (scene->HasSkybox())
        updateSkyboxProxy(scene->GetSkyboxObjectId(), newProxyMap);

    for (const uint32_t sceneLightId : scene->GetSceneLightIds())
        updateSceneLightProxies(sceneLightId, newProxyMap);

    for (const uint32_t sceneObjectId : scene->GetSceneObjectIds())
        updateSceneObjectProxy(sceneObjectId, nullptr, newProxyMap);

    m_Proxies = newProxyMap;
}

Ref<Proxy> ProxyManager::GetProxy(const uint32_t id)
{
    if (m_Proxies.contains(id))
        return m_Proxies[id];

    return nullptr;
}

std::vector<Ref<SceneObjectProxy>> ProxyManager::GetSceneObjectsToRender(const Ref<Scene>& scene)
{
    std::vector<Ref<SceneObjectProxy>> returnVector;
    for (const uint32_t sceneObjectId : scene->GetSceneObjectIds())
        addSceneObjectProxyAndChildrenToList(returnVector, ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId));

    return returnVector;
}

void ProxyManager::updateSceneObjectProxy(const uint32_t sceneObjectId, const Ref<SceneObjectProxy>& parentProxy,
                                          std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap)
{
    const auto sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);
    const auto meshComponent = ECSRegistry::GetInstance().GetComponent<MeshComponent>(sceneObjectId);
    const auto transformComponent = ECSRegistry::GetInstance().GetComponent<TransformComponent>(sceneObjectId);
    const auto materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(sceneObjectId);

    const auto& childEntities = sceneObject->GetChildEntities();
    if (sceneObject->GetDirtyFlag()) // Do we have to update the Proxy?
    {
        for (const auto& childEntity : childEntities)
            childEntity->SetDirtyFlag(true);

        Ref<SceneObjectProxy> sceneObjectProxy;
        if (!m_Proxies.contains(sceneObjectId))
        {
            sceneObjectProxy = CreateRef<SceneObjectProxy>(sceneObjectId);
            proxyMap[sceneObjectId] = sceneObjectProxy;
        }
        else
        {
            sceneObjectProxy = std::static_pointer_cast<SceneObjectProxy>(m_Proxies[sceneObjectId]);
            proxyMap[sceneObjectId] = sceneObjectProxy;
        }

        if (meshComponent)
        {
            Ref<MeshProxy> meshProxy;
            auto meshId = meshComponent->GetMeshAsset()->GetId();
            if (!m_Proxies.contains(meshId))
            {
                meshProxy = CreateRef<MeshProxy>(meshId);
                proxyMap[meshId] = meshProxy;
                meshProxy->CreateBuffers(meshComponent);
            }
            else
            {
                meshProxy = std::static_pointer_cast<MeshProxy>(m_Proxies[meshId]);
                proxyMap[meshId] = meshProxy;
            }
            sceneObjectProxy->SetMesh(meshProxy);
        }
        else
        {
            sceneObjectProxy->SetMesh(nullptr);
        }

        if (materialComponent)
        {
            const uint32_t materialId = materialComponent->GetMaterialAsset()->GetId();
            updateMaterialProxy(materialId, proxyMap);
            sceneObjectProxy->SetMaterial(std::static_pointer_cast<MaterialProxy>(proxyMap[materialId]));
        }
        else
        {
            sceneObjectProxy->SetMaterial(nullptr);
        }

        if (transformComponent)
        {
            sceneObjectProxy->SetTransform(transformComponent->GetPosition(), transformComponent->GetScale(), transformComponent->GetRotation());
            if (parentProxy)
            {
                auto& modelMatrix = sceneObjectProxy->GetModelMatrix();
                modelMatrix = parentProxy->GetModelMatrix() * modelMatrix;
            }
        }
        
        sceneObject->SetDirtyFlag(false);
    }
    else
    {
        proxyMap[sceneObjectId] = m_Proxies[sceneObjectId];
    }

    for (const auto& childEntity : childEntities)
        updateSceneObjectProxy(childEntity->GetId(), std::static_pointer_cast<SceneObjectProxy>(proxyMap[sceneObjectId]), proxyMap);
}

void ProxyManager::updateMaterialProxy(const uint32_t materialId, std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap)
{
    const auto materialAsset = AssetManager::GetInstance().GetMaterial(materialId);

    Ref<MaterialProxy> materialProxy;
    if (!m_Proxies.contains(materialId))
    {
        materialProxy = CreateRef<MaterialProxy>(materialId);
        proxyMap[materialId] = materialProxy;
        materialAsset->SetDirtyFlag(true);
    }
    else
    {
        materialProxy = std::static_pointer_cast<MaterialProxy>(m_Proxies[materialId]);
        proxyMap[materialId] = materialProxy;
    }

    if (!materialAsset->GetDirtyFlag())
        return;

    std::string whitePath("white");
    const auto whiteTextureProxy = AssetManager::GetInstance().LoadTexture(whitePath, false);
    std::string blackPath("black");
    const auto blackTextureProxy = AssetManager::GetInstance().LoadTexture(blackPath, false);
    setupMaterialProxy(materialAsset->GetDiffusePath(), materialProxy->GetDiffuseTexture(),
                       materialAsset->GetDiffuseTextureAsset(), whiteTextureProxy, proxyMap);
    setupMaterialProxy(materialAsset->GetNormalPath(), materialProxy->GetNormalTexture(),
                       materialAsset->GetNormalTextureAsset(), nullptr, proxyMap);
    setupMaterialProxy(materialAsset->GetMetallicPath(), materialProxy->GetMetallicTexture(),
                       materialAsset->GetMetallicTextureAsset(), blackTextureProxy, proxyMap);
    setupMaterialProxy(materialAsset->GetRoughnessPath(), materialProxy->GetRoughnessTexture(),
                       materialAsset->GetRoughnessTextureAsset(), blackTextureProxy, proxyMap);
    setupMaterialProxy(materialAsset->GetAOPath(), materialProxy->GetAOTexture(), materialAsset->GetAOTextureAsset(),
                       whiteTextureProxy, proxyMap);
    setupMaterialProxy(materialAsset->GetEmissivePath(), materialProxy->GetEmissiveTexture(),
                       materialAsset->GetEmissiveTextureAsset(), blackTextureProxy, proxyMap);

    materialAsset->SetDirtyFlag(false);
}

void ProxyManager::updateCameraProxy(const uint32_t cameraId, std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap)
{
    Ref<CameraProxy> cameraProxy;
    if (!m_Proxies.contains(cameraId))
    {
        cameraProxy = CreateRef<CameraProxy>(cameraId);
        proxyMap[cameraId] = cameraProxy;
    }
    else
    {
        cameraProxy = std::static_pointer_cast<CameraProxy>(m_Proxies[cameraId]);
        proxyMap[cameraId] = cameraProxy;
    }
    cameraProxy->UpdateData(ECSRegistry::GetInstance().GetEntity<CameraObject>(cameraId)->GetCameraPtr());
}

void ProxyManager::updateSkyboxProxy(const uint32_t skyboxId, std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap)
{
    const auto skyboxObject = ECSRegistry::GetInstance().GetEntity<SkyboxObject>(skyboxId);

    if (skyboxObject->GetDirtyFlag())
    {
        Ref<SkyboxProxy> skyboxProxy;
        if (!m_Proxies.contains(skyboxId))
        {
            skyboxProxy = CreateRef<SkyboxProxy>(skyboxId);
            proxyMap[skyboxId] = skyboxProxy;
        }
        else
        {
            skyboxProxy = std::static_pointer_cast<SkyboxProxy>(m_Proxies[skyboxId]);
            proxyMap[skyboxId] = skyboxProxy;
        }

        if (skyboxObject->HasAllTexturesSet())
            skyboxProxy->SetTextures(skyboxObject->GetTextureAssets());

        skyboxObject->SetDirtyFlag(false);
    }
    else
    {
        proxyMap[skyboxId] = m_Proxies[skyboxId];
    }
}

void ProxyManager::updateSceneLightProxies(const uint32_t sceneLightId,
                                           std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap)
{
    const auto lightObject = ECSRegistry::GetInstance().GetEntity<LightObject>(sceneLightId);
    if (!lightObject->GetDirtyFlag())
        proxyMap[sceneLightId] = m_Proxies[sceneLightId];

    const bool isDirectionalLight = std::dynamic_pointer_cast<DirectionalLightObject>(lightObject) != nullptr;
    const bool isPointLight = std::dynamic_pointer_cast<PointLightObject>(lightObject) != nullptr;

    if (!m_Proxies.contains(sceneLightId))
    {
        Ref<Proxy> proxy;
        if (isDirectionalLight)
            proxy = CreateRef<DirectionalLightProxy>(sceneLightId);
        else if (isPointLight)
            proxy = CreateRef<PointLightProxy>(sceneLightId);
        proxyMap[sceneLightId] = proxy;
    }
    else
    {
        proxyMap[sceneLightId] = m_Proxies[sceneLightId];
    }
    if (isDirectionalLight)
    {
        const auto dirLightObject = std::static_pointer_cast<DirectionalLightObject>(lightObject);
        const auto proxy = std::static_pointer_cast<DirectionalLightProxy>(proxyMap[sceneLightId]);
        proxy->UpdateData(dirLightObject->GetLightColor(), dirLightObject->GetDirection());
    }
    else if (isPointLight)
    {
        const auto pointLightObject = std::static_pointer_cast<PointLightObject>(lightObject);
        const auto proxy = std::static_pointer_cast<PointLightProxy>(proxyMap[sceneLightId]);
        proxy->UpdateData(pointLightObject->GetLightColor(), pointLightObject->GetPosition(),
                          pointLightObject->GetStrength());
    }
    lightObject->SetDirtyFlag(false);
}

void ProxyManager::setupMaterialProxy(const std::string& assetPath, Ref<TextureProxy>& textureProxy,
                                      const Ref<TextureAsset>& textureAsset,
                                      const Ref<TextureAsset>& alternativeTextureAsset,
                                      std::unordered_map<uint32_t, Ref<Proxy>>& proxyMap)
{
    const Ref<TextureAsset> assetToUse = assetPath.empty() && alternativeTextureAsset ? alternativeTextureAsset : textureAsset;

    if (assetToUse)
    {
        const uint32_t assetId = assetToUse->GetId();
        if (!m_Proxies.contains(assetId))
        {
            textureProxy = CreateRef<TextureProxy>(assetId);
            proxyMap[assetId] = textureProxy;
            textureProxy->CreateTextureFromAsset(assetToUse);
        }
        else
        {
            textureProxy = std::static_pointer_cast<TextureProxy>(m_Proxies[assetId]);
            proxyMap[assetId] = textureProxy;
        }   
    }
}

void ProxyManager::addSceneObjectProxyAndChildrenToList(std::vector<Ref<SceneObjectProxy>>& list,
    const Ref<SceneObject>& sceneObject)
{
    const auto proxy = std::static_pointer_cast<SceneObjectProxy>(m_Proxies[sceneObject->GetId()]);
    if (proxy && proxy->GetMeshProxy())
        list.push_back(proxy);

    for (auto& childObject : sceneObject->GetChildEntities())
        addSceneObjectProxyAndChildrenToList(list, std::static_pointer_cast<SceneObject>(childObject));
}
