#include "ProxyManager.h"
#include "Rendering/Proxy/SceneObjectProxy.h"
#include "Entity/ECSRegistry.h"

ProxyManager::ProxyManager()
    : m_Proxies()
{
}

void ProxyManager::UpdateProxies(Ref<Scene> scene)
{
    for (uint32_t sceneObjectId : scene->GetSceneObjectIds())
    {
        ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);
        if(!m_Proxies.count(sceneObjectId))
            m_Proxies[sceneObjectId] = CreateRef<SceneObjectProxy>(sceneObjectId);
        //Add proxy-properties
    }
}

Ref<Proxy> ProxyManager::GetProxy(uint32_t id)
{
    if (m_Proxies.count(id))
        return m_Proxies[id];

    return nullptr;
}
