#include "ProxyManager.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"

ProxyManager::ProxyManager() = default;

void ProxyManager::UpdateProxies(const Ref<Scene>& scene)
{
    updateCameraProxy(scene->GetCameraId());

    if (scene->HasSkybox())
        updateSkyboxProxy(scene->GetSkyboxObjectId());

    for (const uint32_t sceneLightId : scene->GetSceneLightIds())
        updateSceneLightProxies(sceneLightId);

    for (const uint32_t sceneObjectId : scene->GetSceneObjectIds())
        updateSceneObjectProxy(sceneObjectId, nullptr);
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

void ProxyManager::updateSceneObjectProxy(const uint32_t sceneObjectId, const Ref<SceneObjectProxy>& parentProxy)
{
    const auto sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);
    const auto meshComponent = ECSRegistry::GetInstance().GetComponent<MeshComponent>(sceneObjectId);
    const auto transformComponent = ECSRegistry::GetInstance().GetComponent<TransformComponent>(sceneObjectId);
    const auto materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(sceneObjectId);

    if (!meshComponent || !transformComponent || !materialComponent) // Does the SceneObject have anything to render?
        return;

    const uint32_t materialId = materialComponent->GetMaterialAsset()->GetId();
    updateMaterialProxy(materialId);

    const auto& childEntities = sceneObject->GetChildEntities();
    if (sceneObject->GetDirtyFlag()) // Do we have to update the Proxy?
    {
        for (const auto& childEntity : childEntities)
            childEntity->SetDirtyFlag(true);

        Ref<MeshProxy> meshProxy;
        auto meshId = meshComponent->GetMeshAsset()->GetId();
        if (!m_Proxies.contains(meshId))
        {
            meshProxy = CreateRef<MeshProxy>(meshId);
            m_Proxies[meshId] = meshProxy;
            meshProxy->CreateBuffers(meshComponent);
        }
        else
        {
            meshProxy = std::static_pointer_cast<MeshProxy>(m_Proxies[meshId]);
        }

        Ref<SceneObjectProxy> sceneObjectProxy;
        if (!m_Proxies.contains(sceneObjectId))
        {
            sceneObjectProxy = CreateRef<SceneObjectProxy>(sceneObjectId);
            m_Proxies[sceneObjectId] = sceneObjectProxy;
        }
        else
        {
            sceneObjectProxy = std::static_pointer_cast<SceneObjectProxy>(m_Proxies[sceneObjectId]);
        }

        sceneObjectProxy->SetTransform(transformComponent->GetPosition(), transformComponent->GetScale(),
                                       transformComponent->GetRotation());
        if (parentProxy)
        {
            auto& modelMatrix = sceneObjectProxy->GetModelMatrix();
            modelMatrix = parentProxy->GetModelMatrix() * modelMatrix;
        }

        sceneObjectProxy->SetMesh(meshProxy);
        sceneObject->SetDirtyFlag(false);
        sceneObjectProxy->SetMaterial(std::static_pointer_cast<MaterialProxy>(m_Proxies[materialId]));
    }

    for (const auto& childEntity : childEntities)
        updateSceneObjectProxy(childEntity->GetId(), std::static_pointer_cast<SceneObjectProxy>(m_Proxies[sceneObjectId]));
}

void ProxyManager::updateMaterialProxy(const uint32_t materialId)
{
    const auto materialAsset = AssetManager::GetInstance().GetMaterial(materialId);
    if (!materialAsset->GetDirtyFlag())
        return;

    Ref<MaterialProxy> materialProxy;
    if (!m_Proxies.contains(materialId))
    {
        materialProxy = CreateRef<MaterialProxy>(materialId);
        m_Proxies[materialId] = materialProxy;
    }
    else
    {
        materialProxy = std::static_pointer_cast<MaterialProxy>(m_Proxies[materialId]);
    }
    std::string whitePath("white");
    const auto whiteTextureProxy = AssetManager::GetInstance().LoadTexture(whitePath, false);
    std::string blackPath("black");
    const auto blackTextureProxy = AssetManager::GetInstance().LoadTexture(blackPath, false);
    setupMaterialProxy(materialAsset->GetDiffusePath(), materialProxy->GetDiffuseTexture(),
                       materialAsset->GetDiffuseTextureAsset(), whiteTextureProxy);
    setupMaterialProxy(materialAsset->GetNormalPath(), materialProxy->GetNormalTexture(),
                       materialAsset->GetNormalTextureAsset(), nullptr);
    setupMaterialProxy(materialAsset->GetMetallicPath(), materialProxy->GetMetallicTexture(),
                       materialAsset->GetMetallicTextureAsset(), blackTextureProxy);
    setupMaterialProxy(materialAsset->GetRoughnessPath(), materialProxy->GetRoughnessTexture(),
                       materialAsset->GetRoughnessTextureAsset(), blackTextureProxy);
    setupMaterialProxy(materialAsset->GetAOPath(), materialProxy->GetAOTexture(), materialAsset->GetAOTextureAsset(),
                       whiteTextureProxy);
    setupMaterialProxy(materialAsset->GetEmissivePath(), materialProxy->GetEmissiveTexture(),
                       materialAsset->GetEmissiveTextureAsset(), blackTextureProxy);

    materialAsset->SetDirtyFlag(false);
}

void ProxyManager::updateCameraProxy(const uint32_t cameraId)
{
    Ref<CameraProxy> cameraProxy;
    if (!m_Proxies.contains(cameraId))
    {
        cameraProxy = CreateRef<CameraProxy>(cameraId);
        m_Proxies[cameraId] = cameraProxy;
    }
    else
    {
        cameraProxy = std::static_pointer_cast<CameraProxy>(m_Proxies[cameraId]);
    }
    cameraProxy->UpdateData(ECSRegistry::GetInstance().GetEntity<CameraObject>(cameraId)->GetCameraPtr());
}

void ProxyManager::updateSkyboxProxy(const uint32_t skyboxId)
{
    const auto skyboxObject = ECSRegistry::GetInstance().GetEntity<SkyboxObject>(skyboxId);
    if (skyboxObject->GetDirtyFlag())
    {
        Ref<SkyboxProxy> skyboxProxy;
        if (!m_Proxies.contains(skyboxId))
        {
            skyboxProxy = CreateRef<SkyboxProxy>(skyboxId);
            m_Proxies[skyboxId] = skyboxProxy;
        }
        else
        {
            skyboxProxy = std::static_pointer_cast<SkyboxProxy>(m_Proxies[skyboxId]);
        }

        if (skyboxObject->HasAllTexturesSet())
            skyboxProxy->SetTextures(skyboxObject->GetTextureAssets());

        skyboxObject->SetDirtyFlag(false);
    }
}

void ProxyManager::updateSceneLightProxies(const uint32_t sceneLightId)
{
    const auto lightObject = ECSRegistry::GetInstance().GetEntity<LightObject>(sceneLightId);
    if (!lightObject->GetDirtyFlag())
        return;

    const bool isDirectionalLight = std::dynamic_pointer_cast<DirectionalLightObject>(lightObject) != nullptr;
    const bool isPointLight = std::dynamic_pointer_cast<PointLightObject>(lightObject) != nullptr;

    if (!m_Proxies.contains(sceneLightId))
    {
        Ref<Proxy> proxy;
        if (isDirectionalLight)
            proxy = CreateRef<DirectionalLightProxy>(sceneLightId);
        else if (isPointLight)
            proxy = CreateRef<PointLightProxy>(sceneLightId);
        m_Proxies[sceneLightId] = proxy;
    }
    if (isDirectionalLight)
    {
        const auto dirLightObject = std::static_pointer_cast<DirectionalLightObject>(lightObject);
        const auto proxy = std::static_pointer_cast<DirectionalLightProxy>(m_Proxies[sceneLightId]);
        proxy->UpdateData(dirLightObject->GetLightColor(), dirLightObject->GetDirection());
    }
    else if (isPointLight)
    {
        const auto pointLightObject = std::static_pointer_cast<PointLightObject>(lightObject);
        const auto proxy = std::static_pointer_cast<PointLightProxy>(m_Proxies[sceneLightId]);
        proxy->UpdateData(pointLightObject->GetLightColor(), pointLightObject->GetPosition(),
                          pointLightObject->GetStrength());
    }
    lightObject->SetDirtyFlag(false);
}

void ProxyManager::setupMaterialProxy(const std::string& assetPath, Ref<TextureProxy>& textureProxy,
                                      const Ref<TextureAsset>& textureAsset, const Ref<TextureAsset>& alternativeTextureAsset)
{
    const Ref<TextureAsset> assetToUse = assetPath.empty() && alternativeTextureAsset ? alternativeTextureAsset : textureAsset;

    if (assetToUse)
    {
        const uint32_t assetId = assetToUse->GetId();
        if (!m_Proxies.contains(assetId))
        {
            textureProxy = CreateRef<TextureProxy>(assetId);
            m_Proxies[assetId] = textureProxy;
            textureProxy->CreateTextureFromAsset(assetToUse);
        }
        else
        {
            textureProxy = std::static_pointer_cast<TextureProxy>(m_Proxies[assetId]);
        }   
    }
}

void ProxyManager::addSceneObjectProxyAndChildrenToList(std::vector<Ref<SceneObjectProxy>>& list,
    const Ref<SceneObject>& sceneObject)
{
    const auto proxy = std::static_pointer_cast<SceneObjectProxy>(m_Proxies[sceneObject->GetId()]);
    if (proxy)
        list.push_back(proxy);

    for (auto& childObject : sceneObject->GetChildEntities())
        addSceneObjectProxyAndChildrenToList(list, std::static_pointer_cast<SceneObject>(childObject));
}
