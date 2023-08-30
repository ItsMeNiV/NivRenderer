#include "ProxyManager.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"

ProxyManager::ProxyManager()
{}

void ProxyManager::UpdateProxies(const Ref<Scene>& scene)
{
    //Lights
    for (uint32_t sceneLightId : scene->GetSceneLightIds())
    {
        auto lightObject = ECSRegistry::GetInstance().GetEntity<LightObject>(sceneLightId);
        if (!lightObject->GetDirtyFlag())
            continue;

        const bool isDirectionalLight = std::dynamic_pointer_cast<DirectionalLightObject>(lightObject) != nullptr;
        const bool isPointLight = std::dynamic_pointer_cast<PointLightObject>(lightObject) != nullptr;

        if (!m_Proxies.contains(sceneLightId))
        {
            Ref<Proxy> proxy;
            if(isDirectionalLight)
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
            proxy->UpdateData(pointLightObject->GetLightColor(), pointLightObject->GetPosition(), pointLightObject->GetStrength());
        }
        lightObject->SetDirtyFlag(false);
    }

    //SceneObjects
    for (uint32_t sceneObjectId : scene->GetSceneObjectIds())
    {
        const auto sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);
        if (!sceneObject->GetDirtyFlag())
            continue;

        // Mesh Proxy
        auto mesh = ECSRegistry::GetInstance().GetComponent<MeshComponent>(sceneObjectId);
        auto meshId = mesh->GetMeshAsset()->GetId();
        Ref<MeshProxy> meshProxy;
        if (!m_Proxies.contains(meshId))
        {
            meshProxy = CreateRef<MeshProxy>(meshId);
            m_Proxies[meshId] = meshProxy;
            meshProxy->CreateBuffers(mesh);
        }
        else
        {
            meshProxy = std::static_pointer_cast<MeshProxy>(m_Proxies[meshId]);
        }

        // Material / Texture Proxies
        const auto material = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(sceneObjectId);
        auto materialId = material->GetId();
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
        auto whiteTextureProxy = AssetManager::GetInstance().LoadTexture(whitePath, false);
        std::string blackPath("black");
        auto blackTextureProxy = AssetManager::GetInstance().LoadTexture(blackPath, false);
        setupMaterialProxy(material->GetDiffusePath(), materialProxy->GetDiffuseTexture(), material->GetDiffuseTextureAsset(), whiteTextureProxy);
        setupMaterialProxy(material->GetNormalPath(), materialProxy->GetNormalTexture(), material->GetNormalTextureAsset(), nullptr);
        setupMaterialProxy(material->GetMetallicPath(), materialProxy->GetMetallicTexture(), material->GetMetallicTextureAsset(), blackTextureProxy);
        setupMaterialProxy(material->GetRoughnessPath(), materialProxy->GetRoughnessTexture(), material->GetRoughnessTextureAsset(), blackTextureProxy);
        setupMaterialProxy(material->GetAOPath(), materialProxy->GetAOTexture(), material->GetAOTextureAsset(), whiteTextureProxy);
        setupMaterialProxy(material->GetEmissivePath(), materialProxy->GetEmissiveTexture(), material->GetEmissiveTextureAsset(), blackTextureProxy);

        // Scene Object Proxy
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
        const auto transform = ECSRegistry::GetInstance().GetComponent<TransformComponent>(sceneObjectId);
        sceneObjectProxy->SetTransform(transform->GetPosition(), transform->GetScale(), transform->GetRotation());
        sceneObjectProxy->SetMesh(meshProxy);
        sceneObjectProxy->SetMaterial(materialProxy);

        sceneObject->SetDirtyFlag(false);
    }

    //Camera
    Ref<CameraProxy> cameraProxy;
    if (!m_Proxies.contains(scene->GetCameraId()))
    {
        cameraProxy = CreateRef<CameraProxy>(scene->GetCameraId());
        m_Proxies[scene->GetCameraId()] = cameraProxy;
    }
    else
    {
        cameraProxy = std::static_pointer_cast<CameraProxy>(m_Proxies[scene->GetCameraId()]);
    }
    cameraProxy->UpdateData(ECSRegistry::GetInstance().GetEntity<CameraObject>(scene->GetCameraId())->GetCameraPtr());

    //Skybox
    if(scene->HasSkybox())
    {
        const uint32_t skyboxId = scene->GetSkyboxObjectId();
        auto skyboxObject = ECSRegistry::GetInstance().GetEntity<SkyboxObject>(skyboxId);
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
}

Ref<Proxy> ProxyManager::GetProxy(const uint32_t id)
{
    if (m_Proxies.contains(id))
        return m_Proxies[id];

    return nullptr;
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
