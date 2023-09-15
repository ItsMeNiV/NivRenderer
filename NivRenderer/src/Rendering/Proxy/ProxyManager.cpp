#include "ProxyManager.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"

ProxyManager::ProxyManager() = default;

void ProxyManager::UpdateProxies(const Scene* const scene)
{
    updateCameraProxy(scene->GetCameraId());

    if (scene->HasSkybox())
        updateSkyboxProxy(scene->GetSkyboxObjectId());

    for (const uint32_t sceneLightId : scene->GetSceneLightIds())
        updateSceneLightProxies(sceneLightId);

    for (const uint32_t sceneObjectId : scene->GetSceneObjectIds())
        updateSceneObjectProxy(sceneObjectId, nullptr);
}

Proxy* ProxyManager::GetProxy(const uint32_t id)
{
    if (m_Proxies.contains(id))
        return m_Proxies[id].get();

    return nullptr;
}

std::vector<SceneObjectProxy*> ProxyManager::GetSceneObjectsToRender(const Scene* const scene)
{
    std::vector<SceneObjectProxy*> returnVector;
    for (const uint32_t sceneObjectId : scene->GetSceneObjectIds())
        addSceneObjectProxyAndChildrenToList(returnVector, ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId));

    return returnVector;
}

void ProxyManager::updateSceneObjectProxy(const uint32_t sceneObjectId, SceneObjectProxy* const parentProxy)
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

        if (!m_Proxies.contains(sceneObjectId))
        {
            m_Proxies[sceneObjectId] = CreateScope<SceneObjectProxy>(sceneObjectId);
        }
        auto* sceneObjectProxy = dynamic_cast<SceneObjectProxy*>(m_Proxies[sceneObjectId].get());

        if (meshComponent)
        {

            MeshProxy* meshProxy;
            auto meshId = meshComponent->GetMeshAsset()->GetId();
            if (!m_Proxies.contains(meshId))
            {
                m_Proxies[meshId] = CreateScope<MeshProxy>(meshId);
                meshProxy = dynamic_cast<MeshProxy*>(m_Proxies[meshId].get());
                meshProxy->CreateBuffers(meshComponent);
            }
            else
            {
                meshProxy = dynamic_cast<MeshProxy*>(m_Proxies[meshId].get());
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
            updateMaterialProxy(materialId);
            sceneObjectProxy->SetMaterial(dynamic_cast<MaterialProxy*>(m_Proxies[materialId].get()));
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

    for (const auto& childEntity : childEntities)
        updateSceneObjectProxy(childEntity->GetId(), dynamic_cast<SceneObjectProxy*>(m_Proxies[sceneObjectId].get()));
}

void ProxyManager::updateMaterialProxy(const uint32_t materialId)
{
    const auto materialAsset = AssetManager::GetInstance().GetMaterial(materialId);

    if (!m_Proxies.contains(materialId))
    {
        m_Proxies[materialId] = CreateScope<MaterialProxy>(materialId);
    }
    const auto materialProxy = dynamic_cast<MaterialProxy*>(m_Proxies[materialId].get());

    if (!materialAsset->GetDirtyFlag())
        return;

    std::string whitePath("white");
    const auto whiteTextureProxy = AssetManager::GetInstance().LoadTexture(whitePath, false);
    std::string blackPath("black");
    const auto blackTextureProxy = AssetManager::GetInstance().LoadTexture(blackPath, false);
    setupMaterialProxy(materialAsset->GetDiffusePath(), materialProxy->GetDiffuseTexture(),
                       *materialAsset->GetDiffuseTextureAsset(), whiteTextureProxy);
    setupMaterialProxy(materialAsset->GetNormalPath(), materialProxy->GetNormalTexture(),
                       *materialAsset->GetNormalTextureAsset(), nullptr);
    setupMaterialProxy(materialAsset->GetMetallicPath(), materialProxy->GetMetallicTexture(),
                       *materialAsset->GetMetallicTextureAsset(), blackTextureProxy);
    setupMaterialProxy(materialAsset->GetRoughnessPath(), materialProxy->GetRoughnessTexture(),
                       *materialAsset->GetRoughnessTextureAsset(), blackTextureProxy);
    setupMaterialProxy(materialAsset->GetAOPath(), materialProxy->GetAOTexture(),
                       *materialAsset->GetAOTextureAsset(), whiteTextureProxy);
    setupMaterialProxy(materialAsset->GetEmissivePath(), materialProxy->GetEmissiveTexture(),
                       *materialAsset->GetEmissiveTextureAsset(), blackTextureProxy);

    materialAsset->SetDirtyFlag(false);
}

void ProxyManager::updateCameraProxy(const uint32_t cameraId)
{
    if (!m_Proxies.contains(cameraId))
    {
        m_Proxies[cameraId] = CreateScope<CameraProxy>(cameraId);
    }
    CameraProxy* cameraProxy = dynamic_cast<CameraProxy*>(m_Proxies[cameraId].get());
    cameraProxy->UpdateData(ECSRegistry::GetInstance().GetEntity<CameraObject>(cameraId)->GetCameraPtr());
}

void ProxyManager::updateSkyboxProxy(const uint32_t skyboxId)
{
    const auto skyboxObject = ECSRegistry::GetInstance().GetEntity<SkyboxObject>(skyboxId);

    if (skyboxObject->GetDirtyFlag())
    {
        if (!m_Proxies.contains(skyboxId))
        {
            m_Proxies[skyboxId] = CreateScope<SkyboxProxy>(skyboxId);
        }
        auto skyboxProxy = dynamic_cast<SkyboxProxy*>(m_Proxies[skyboxId].get());

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

    const auto directionalLight = dynamic_cast<DirectionalLightObject*>(lightObject);
    const auto pointLight = dynamic_cast<PointLightObject*>(lightObject);

    if (!m_Proxies.contains(sceneLightId))
    {
        if (directionalLight)
            m_Proxies[sceneLightId] = CreateScope<DirectionalLightProxy>(sceneLightId);
        else if (pointLight)
            m_Proxies[sceneLightId] = CreateScope<PointLightProxy>(sceneLightId);
    }
    if (directionalLight)
    {
        dynamic_cast<DirectionalLightProxy*>(m_Proxies[sceneLightId].get())
            ->UpdateData(directionalLight->GetLightColor(), directionalLight->GetDirection());
    }
    else if (pointLight)
    {
        dynamic_cast<PointLightProxy*>(m_Proxies[sceneLightId].get())
            ->UpdateData(pointLight->GetLightColor(), pointLight->GetPosition(), pointLight->GetStrength());
    }
    lightObject->SetDirtyFlag(false);
}

void ProxyManager::setupMaterialProxy(const std::string& assetPath, TextureProxy** const textureProxy,
                                      TextureAsset* const textureAsset,
                                      TextureAsset* const alternativeTextureAsset)
{
    const auto assetToUse = assetPath.empty() && alternativeTextureAsset ? alternativeTextureAsset : textureAsset;

    if (assetToUse)
    {
        const uint32_t assetId = assetToUse->GetId();
        if (!m_Proxies.contains(assetId))
        {
            m_Proxies[assetId] = CreateScope<TextureProxy>(assetId);
            *textureProxy = dynamic_cast<TextureProxy*>(m_Proxies[assetId].get());
            (*textureProxy)->CreateTextureFromAsset(assetToUse);
        }
    }
}

void ProxyManager::addSceneObjectProxyAndChildrenToList(std::vector<SceneObjectProxy*>& list,
    const SceneObject* const sceneObject)
{
    const auto proxy = static_cast<SceneObjectProxy*>(m_Proxies[sceneObject->GetId()].get());
    if (proxy && proxy->GetMeshProxy())
        list.push_back(proxy);

    for (const auto childObject : sceneObject->GetChildEntities())
        addSceneObjectProxyAndChildrenToList(list, static_cast<SceneObject*>(childObject));
}
