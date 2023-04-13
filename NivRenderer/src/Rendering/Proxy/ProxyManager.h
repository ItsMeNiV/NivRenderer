#pragma once
#include "Base.h"
#include "Application/Scene.h"
#include "Rendering/Proxy/Proxy.h"

class ProxyManager
{
public:
    ProxyManager();

    void UpdateProxies(Ref<Scene> scene);
    Ref<Proxy> GetProxy(uint32_t id);

private:
    std::unordered_map<uint32_t, Ref<Proxy>> m_Proxies;
};