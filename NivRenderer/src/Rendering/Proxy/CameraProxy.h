#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"

class CameraProxy : public Proxy
{
public:
    CameraProxy(uint32_t id) : Proxy(id) {}
    virtual ~CameraProxy() = default;

    void UpdateData(Ref<Camera> camera)
    {
        m_View = camera->GetView();
        m_Projection = camera->GetProjection();
    }

    const glm::mat4& GetView() const { return m_View; }
    const glm::mat4& GetProjection() const { return m_Projection; }

private:
    glm::mat4 m_View;
    glm::mat4 m_Projection;
};