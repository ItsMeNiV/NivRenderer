#include "ProxyManager.h"
#include "Rendering/Proxy/SceneObjectProxy.h"
#include "Rendering/Proxy/LightProxy.h"
#include "Rendering/Proxy/CameraProxy.h"
#include "Entity/ECSRegistry.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"

ProxyManager::ProxyManager()
    : m_Proxies()
{
}

void ProxyManager::UpdateProxies(Ref<Scene> scene)
{
    for (uint32_t sceneLightId : scene->GetSceneLightIds())
    {
        auto lightObject = ECSRegistry::GetInstance().GetEntity<LightObject>(sceneLightId);
        if (!lightObject->GetDirtyFlag())
            continue;

        bool isDirectionalLight = std::dynamic_pointer_cast<DirectionalLightObject>(lightObject) != 0;

        if (!m_Proxies.count(sceneLightId))
        {
            Ref<Proxy> proxy;
            if(isDirectionalLight)
                proxy = CreateRef<DirectionalLightProxy>(sceneLightId);
            m_Proxies[sceneLightId] = proxy;
        }
        if (isDirectionalLight)
        {
            Ref<DirectionalLightObject> dirLightObject = std::static_pointer_cast<DirectionalLightObject>(lightObject);
            Ref<DirectionalLightProxy> proxy = std::static_pointer_cast<DirectionalLightProxy>(m_Proxies[sceneLightId]);
            proxy->SetLightColor(dirLightObject->GetLightColor());
            proxy->SetLightDirection(dirLightObject->GetDirection());
        }
        lightObject->SetDirtyFlag(false);
    }

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
        Ref<SceneObjectProxy> proxy = std::static_pointer_cast<SceneObjectProxy>(m_Proxies[sceneObjectId]);
        Ref<TransformComponent> transform = ECSRegistry::GetInstance().GetComponent<TransformComponent>(sceneObjectId);
        Ref<MeshComponent> mesh = ECSRegistry::GetInstance().GetComponent<MeshComponent>(sceneObjectId);
        proxy->SetTransform(transform->GetPosition(), transform->GetScale(), transform->GetRotation());
        proxy->SetMesh(mesh);

        sceneObject->SetDirtyFlag(false);
    }

    if (!m_Proxies.count(scene->GetCameraId()))
    {
        Ref<CameraProxy> proxy = CreateRef<CameraProxy>(scene->GetCameraId());
        m_Proxies[scene->GetCameraId()] = proxy;
    }
    Ref<CameraProxy> proxy = std::static_pointer_cast<CameraProxy>(m_Proxies[scene->GetCameraId()]);
    proxy->UpdateData(ECSRegistry::GetInstance().GetEntity<CameraObject>(scene->GetCameraId())->GetCameraPtr());
}

Ref<Proxy> ProxyManager::GetProxy(uint32_t id)
{
    if (m_Proxies.count(id))
        return m_Proxies[id];

    return nullptr;
}
