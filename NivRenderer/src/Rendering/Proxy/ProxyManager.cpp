#include "ProxyManager.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components.h"
#include "Entity/Assets/AssetManager.h"

ProxyManager::ProxyManager() = default;

void ProxyManager::UpdateProxies(Scene* const scene)
{
    updateCameraProxy(scene->GetActiveCameraId());

    if (scene->HasSkybox())
        updateSkyboxProxy(scene->GetSkyboxObjectId());

    if (scene->HasDirectionalLight())
        updateSceneLightProxy(scene->GetDirectionalLightId(), true);
    for (const uint32_t sceneLightId : scene->GetPointLightIds())
        updateSceneLightProxy(sceneLightId, false);

    m_SceneObjectsToRender.clear();
    m_SceneObjectsToRenderByMaterial.clear();
    for (const uint32_t sceneObjectId : scene->GetSceneObjectIds())
    {
        if (scene->GetSceneHierarchyElementById(sceneObjectId)->parentId == UINT32_MAX)
            updateSceneObjectProxy(scene, sceneObjectId, nullptr);
    }
}

Proxy* ProxyManager::GetProxy(const uint32_t id)
{
    if (m_Proxies.contains(id))
        return m_Proxies[id].get();

    return nullptr;
}

std::vector<SceneObjectProxy*> ProxyManager::GetSceneObjectsToRender(Scene* const scene)
{
    return m_SceneObjectsToRender;
}

std::unordered_map<uint32_t, std::vector<SceneObjectProxy*>> ProxyManager::GetSceneObjectsToRenderByMaterial(const Scene* const scene)
{
    return m_SceneObjectsToRenderByMaterial;
}

void ProxyManager::Reset() { m_Proxies.clear(); }

void ProxyManager::updateSceneObjectProxy(Scene* const scene, const uint32_t sceneObjectId,
                                          SceneObjectProxy* const parentProxy)
{
    const auto sceneObjectComponent = ECSRegistry::GetInstance().GetComponent<SceneObjectComponent>(sceneObjectId);
    const auto meshComponent = ECSRegistry::GetInstance().GetComponent<MeshComponent>(sceneObjectId);
    const auto transformComponent = ECSRegistry::GetInstance().GetComponent<TransformComponent>(sceneObjectId);
    const auto materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(sceneObjectId);

    const auto& childIds = scene->GetSceneHierarchyElementById(sceneObjectId)->childIds;
    if (sceneObjectComponent->dirtyFlag) // Do we have to update the Proxy?
    {
        for (const auto& childId : childIds)
            ECSRegistry::GetInstance().GetComponent<SceneObjectComponent>(childId)->dirtyFlag = true;

        if (!m_Proxies.contains(sceneObjectId))
        {
            m_Proxies[sceneObjectId] = CreateScope<SceneObjectProxy>(sceneObjectId);
        }
        auto* sceneObjectProxy = dynamic_cast<SceneObjectProxy*>(m_Proxies[sceneObjectId].get());

        if (meshComponent)
        {

            MeshProxy* meshProxy;
            auto meshId = meshComponent->meshAsset->GetId();
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
            const uint32_t materialId = materialComponent->materialAsset->GetId();
            updateMaterialProxy(materialId);
            sceneObjectProxy->SetMaterial(dynamic_cast<MaterialProxy*>(m_Proxies[materialId].get()));
        }
        else
        {
            sceneObjectProxy->SetMaterial(nullptr);
        }

        if (transformComponent)
        {
            sceneObjectProxy->SetTransform(transformComponent->position, transformComponent->scale, transformComponent->rotation);
            if (parentProxy)
            {
                auto& modelMatrix = sceneObjectProxy->GetModelMatrix();
                modelMatrix = parentProxy->GetModelMatrix() * modelMatrix;
            }
        }
        
        sceneObjectComponent->dirtyFlag = false;
        sceneObjectProxy->GetDirtyFlag() = true;
    }

    for (const auto& childId : childIds)
        updateSceneObjectProxy(scene, childId, dynamic_cast<SceneObjectProxy*>(m_Proxies[sceneObjectId].get()));

    if (meshComponent)
    {
        m_SceneObjectsToRender.push_back(dynamic_cast<SceneObjectProxy*>(m_Proxies[sceneObjectId].get()));
        m_SceneObjectsToRenderByMaterial[materialComponent->materialAsset->GetId()].push_back(dynamic_cast<SceneObjectProxy*>(m_Proxies[sceneObjectId].get()));
    }
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
    setupMaterialProxy(materialAsset->GetDiffusePath(), materialProxy->GetDiffuseTexturePtr(),
                       *materialAsset->GetDiffuseTextureAsset(), whiteTextureProxy);
    setupMaterialProxy(materialAsset->GetNormalPath(), materialProxy->GetNormalTexturePtr(),
                       *materialAsset->GetNormalTextureAsset(), nullptr);
    setupMaterialProxy(materialAsset->GetMetallicPath(), materialProxy->GetMetallicTexturePtr(),
                       *materialAsset->GetMetallicTextureAsset(), blackTextureProxy);
    setupMaterialProxy(materialAsset->GetRoughnessPath(), materialProxy->GetRoughnessTexturePtr(),
                       *materialAsset->GetRoughnessTextureAsset(), blackTextureProxy);
    setupMaterialProxy(materialAsset->GetAOPath(), materialProxy->GetAOTexturePtr(),
                       *materialAsset->GetAOTextureAsset(), whiteTextureProxy);
    setupMaterialProxy(materialAsset->GetEmissivePath(), materialProxy->GetEmissiveTexturePtr(),
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
    cameraProxy->UpdateData(ECSRegistry::GetInstance().GetComponent<CameraComponent>(cameraId)->cameraPtr);
}

void ProxyManager::updateSkyboxProxy(const uint32_t skyboxId)
{
    const auto skyboxComponent = ECSRegistry::GetInstance().GetComponent<SkyboxComponent>(skyboxId);

    if (!skyboxComponent->dirtyFlag)
        return;

    if (!m_Proxies.contains(skyboxId))
    {
        m_Proxies[skyboxId] = CreateScope<SkyboxProxy>(skyboxId);
    }
    const auto skyboxProxy = dynamic_cast<SkyboxProxy*>(m_Proxies[skyboxId].get());

    bool hasAllTexturesSet = true;
    for (const auto path : skyboxComponent->texturePaths)
    {
        if (path.empty())
        {
            hasAllTexturesSet = false;
            break;
        }
    }
    if (hasAllTexturesSet)
        skyboxProxy->SetTextures(skyboxComponent->textureAssets);

    skyboxComponent->dirtyFlag = false;
    skyboxProxy->GetDirtyFlag() = true;
}

void ProxyManager::updateSceneLightProxy(const uint32_t sceneLightId, const bool isDirectionalLight)
{
    if (isDirectionalLight)
    {
        const auto directionalLightComponent =
            ECSRegistry::GetInstance().GetComponent<DirectionalLightComponent>(sceneLightId);
        if (!directionalLightComponent->dirtyFlag)
            return;

        if (!m_Proxies.contains(sceneLightId))
            m_Proxies[sceneLightId] = CreateScope<DirectionalLightProxy>(sceneLightId);

        dynamic_cast<DirectionalLightProxy*>(m_Proxies[sceneLightId].get())
            ->UpdateData(directionalLightComponent->lightColor, directionalLightComponent->direction);

        directionalLightComponent->dirtyFlag = false;
        m_Proxies[sceneLightId]->GetDirtyFlag() = true;
    }
    else
    {
        const auto pointLightComponent =
            ECSRegistry::GetInstance().GetComponent<PointLightComponent>(sceneLightId);
        if (!pointLightComponent->dirtyFlag)
            return;

        if (!m_Proxies.contains(sceneLightId))
            m_Proxies[sceneLightId] = CreateScope<PointLightProxy>(sceneLightId);

        dynamic_cast<PointLightProxy*>(m_Proxies[sceneLightId].get())
            ->UpdateData(pointLightComponent->lightColor, pointLightComponent->position,
                         pointLightComponent->strength);

        pointLightComponent->dirtyFlag = false;
        m_Proxies[sceneLightId]->GetDirtyFlag() = true;
    }
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
        else
        {
            *textureProxy = dynamic_cast<TextureProxy*>(m_Proxies[assetId].get());
        }
    }
}
