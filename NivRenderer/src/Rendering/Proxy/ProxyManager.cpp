#include "ProxyManager.h"
#include "Rendering/Proxy/SceneObjectProxy.h"
#include "Rendering/Proxy/LightProxy.h"
#include "Rendering/Proxy/CameraProxy.h"
#include "Rendering/Proxy/SkyboxProxy.h"
#include "Entity/ECSRegistry.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"

ProxyManager::ProxyManager()
    : m_Proxies()
{
}

void ProxyManager::UpdateProxies(Ref<Scene> scene)
{
    //Lights
    for (uint32_t sceneLightId : scene->GetSceneLightIds())
    {
        auto lightObject = ECSRegistry::GetInstance().GetEntity<LightObject>(sceneLightId);
        if (!lightObject->GetDirtyFlag())
            continue;

        bool isDirectionalLight = std::dynamic_pointer_cast<DirectionalLightObject>(lightObject) != 0;
        bool isPointLight = std::dynamic_pointer_cast<PointLightObject>(lightObject) != 0;

        if (!m_Proxies.count(sceneLightId))
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
            auto dirLightObject = std::static_pointer_cast<DirectionalLightObject>(lightObject);
            auto proxy = std::static_pointer_cast<DirectionalLightProxy>(m_Proxies[sceneLightId]);
            proxy->UpdateData(dirLightObject->GetLightColor(), dirLightObject->GetDirection());
        }
        else if (isPointLight)
        {
            auto pointLightObject = std::static_pointer_cast<PointLightObject>(lightObject);
            auto proxy = std::static_pointer_cast<PointLightProxy>(m_Proxies[sceneLightId]);
            proxy->UpdateData(pointLightObject->GetLightColor(), pointLightObject->GetPosition(), pointLightObject->GetStrength());
        }
        lightObject->SetDirtyFlag(false);
    }

    //SceneObjects
    for (uint32_t sceneObjectId : scene->GetSceneObjectIds())
    {
        auto sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);
        if (!sceneObject->GetDirtyFlag())
            continue;

        if (!m_Proxies.count(sceneObjectId))
        {
            Ref<SceneObjectProxy> proxy = CreateRef<SceneObjectProxy>(sceneObjectId);
            m_Proxies[sceneObjectId] = proxy;
        }
        auto proxy = std::static_pointer_cast<SceneObjectProxy>(m_Proxies[sceneObjectId]);
        auto transform = ECSRegistry::GetInstance().GetComponent<TransformComponent>(sceneObjectId);
        auto mesh = ECSRegistry::GetInstance().GetComponent<MeshComponent>(sceneObjectId);
        auto material = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(sceneObjectId);
        proxy->SetTransform(transform->GetPosition(), transform->GetScale(), transform->GetRotation());
        proxy->SetMesh(mesh);
        proxy->SetMaterial(material);

        sceneObject->SetDirtyFlag(false);
    }

    //Camera
    if (!m_Proxies.count(scene->GetCameraId()))
    {
        Ref<CameraProxy> proxy = CreateRef<CameraProxy>(scene->GetCameraId());
        m_Proxies[scene->GetCameraId()] = proxy;
    }
    Ref<CameraProxy> proxy = std::static_pointer_cast<CameraProxy>(m_Proxies[scene->GetCameraId()]);
    proxy->UpdateData(ECSRegistry::GetInstance().GetEntity<CameraObject>(scene->GetCameraId())->GetCameraPtr());

    //Skybox
    if(scene->HasSkybox())
    {
        uint32_t skyboxId = scene->GetSkyboxObjectId();
        auto skyboxObject = ECSRegistry::GetInstance().GetEntity<SkyboxObject>(skyboxId);
        if (skyboxObject->GetDirtyFlag())
        {
            Ref<SkyboxProxy> proxy;
            if (!m_Proxies.count(skyboxId))
            {
                Ref<SkyboxProxy> proxy = CreateRef<SkyboxProxy>(skyboxId);
                m_Proxies[skyboxId] = proxy;
            }
            else
            {
                proxy = std::static_pointer_cast<SkyboxProxy>(m_Proxies[skyboxId]);
            }

            if (skyboxObject->HasAllTexturesSet())
                proxy->SetTextures(skyboxObject->GetTextureAssets());

            skyboxObject->SetDirtyFlag(false);
        }
    }
}

Ref<Proxy> ProxyManager::GetProxy(uint32_t id)
{
    if (m_Proxies.count(id))
        return m_Proxies[id];

    return nullptr;
}
