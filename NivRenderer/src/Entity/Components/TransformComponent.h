#pragma once
#include "Entity/Component.h"

class TransformComponent : public Component
{
public:
    TransformComponent(const uint32_t id)
		: Component(id, "TransformComponent"),
		m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f)
	{}

	~TransformComponent()
	{}

    std::vector<std::pair<std::string, Property>> GetComponentProperties() override
	{
        std::vector<std::pair<std::string, Property>> returnVector;

        returnVector.push_back({"Position", {PropertyType::FLOAT3, glm::value_ptr(m_Position)}});;
        returnVector.push_back({"Scale", {PropertyType::FLOAT3, glm::value_ptr(m_Scale)}});
        returnVector.push_back({"Rotation", {PropertyType::FLOAT3, glm::value_ptr(m_Rotation)}});

        return returnVector;
	}

	glm::vec3& GetPosition() { return m_Position; }
	glm::vec3& GetScale() { return m_Scale; }
	glm::vec3& GetRotation() { return m_Rotation; }

private:
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
};