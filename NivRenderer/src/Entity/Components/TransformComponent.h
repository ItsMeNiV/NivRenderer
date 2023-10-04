#pragma once
#include "Entity/Component.h"
#include "json.hpp"

class TransformComponent : public Component
{
public:
    TransformComponent(const uint32_t id);

	~TransformComponent() override;

    std::vector<std::pair<std::string, Property>> GetComponentProperties() override;

	glm::vec3& GetPosition();
    glm::vec3& GetScale();
    glm::vec3& GetRotation();

    nlohmann::ordered_json SerializeObject() override;
    void DeSerializeObject(nlohmann::json jsonObject);

private:
	glm::vec3 m_Position;
    glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
    glm::vec3 m_DegRotation;
};