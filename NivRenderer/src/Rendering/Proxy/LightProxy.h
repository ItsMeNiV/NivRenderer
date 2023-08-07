#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"

class DirectionalLightProxy : public Proxy
{
public:
    DirectionalLightProxy(uint32_t id) : Proxy(id) {}
    virtual ~DirectionalLightProxy() = default;

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

class PointLightProxy : public Proxy
{
public:
    PointLightProxy(uint32_t id) : Proxy(id) {}
    virtual ~PointLightProxy() = default;

    void UpdateData(glm::vec3 lightColor, glm::vec3 position, uint32_t strength)
    {
        m_LightColor = lightColor;
        m_LightPosition = position;
        m_LightStrength = strength;
    }

    const glm::vec3& GetLightColor() const { return m_LightColor; }
    const glm::vec3& GetLightPosition() const { return m_LightPosition; }
    const uint32_t& GetLightStrength() const { return m_LightStrength; }

    void SetLightColor(const glm::vec3& color) { m_LightColor = color; }
    void SetLightPosition(const glm::vec3& position) { m_LightPosition = position; }
    void SetLightStrength(const uint32_t& strength) { m_LightStrength = strength; }

private:
    glm::vec3 m_LightColor;
    glm::vec3 m_LightPosition;
    uint32_t m_LightStrength;
};