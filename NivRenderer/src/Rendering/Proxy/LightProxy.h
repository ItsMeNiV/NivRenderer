#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"

class DirectionalLightProxy : public Proxy
{
public:
    DirectionalLightProxy(uint32_t id) : Proxy(id) {}

    void UpdateData(glm::vec3 lightColor, glm::vec3 direction)
    {
        m_LightColor = lightColor;
        m_LightDirection = direction;
    }

    const glm::vec3& GetLightColor() const { return m_LightColor; }
    const glm::vec3& GetLightDirection() const { return m_LightDirection; }

    void SetLightColor(const glm::vec3& color) { m_LightColor = color; }
    void SetLightDirection(const glm::vec3& direction) { m_LightDirection = direction; }

private:
    glm::vec3 m_LightColor;
    glm::vec3 m_LightDirection;
};