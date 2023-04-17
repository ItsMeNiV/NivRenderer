#include "ProxyManager.h"
#include "Rendering/Proxy/SceneObjectProxy.h"
#include "Entity/ECSRegistry.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"

ProxyManager::ProxyManager()
    : m_Proxies()
{
}

void ProxyManager::UpdateProxies(Ref<Scene> scene)
{
    for (uint32_t sceneObjectId : scene->GetSceneObjectIds())
    {
        auto sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);
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
    }
}

Ref<Proxy> ProxyManager::GetProxy(uint32_t id)
{
    if (m_Proxies.count(id))
        return m_Proxies[id];

    return nullptr;
}
