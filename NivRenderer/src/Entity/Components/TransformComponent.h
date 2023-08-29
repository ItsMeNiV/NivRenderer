#pragma once
#include "Entity/Component.h"

class TransformComponent : public Component
{
public:
	TransformComponent()
		: Component("TransformComponent"),
		m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f)
	{}

	~TransformComponent()
	{}

    std::vector<std::pair<std::string, ComponentProperty>> GetComponentProperties() override
	{
        std::vector<std::pair<std::string, ComponentProperty>> returnVector;

        returnVector.push_back({"Position", {NivRenderer::PropertyType::FLOAT3, glm::value_ptr(m_Position)}});;
        returnVector.push_back({"Scale", {NivRenderer::PropertyType::FLOAT3, glm::value_ptr(m_Scale)}});
        returnVector.push_back({"Rotation", {NivRenderer::PropertyType::FLOAT3, glm::value_ptr(m_Rotation)}});

        return returnVector;
	}

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetScale() const { return m_Scale; }
	const glm::vec3& GetRotation() const { return m_Rotation; }

private:
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
};