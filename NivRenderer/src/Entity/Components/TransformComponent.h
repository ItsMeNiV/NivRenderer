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

	virtual std::unordered_map<std::string, ComponentProperty> GetComponentProperties()
	{
		std::unordered_map<std::string, ComponentProperty> returnMap;

		returnMap["Position"] = { PropertyType::FLOAT3, glm::value_ptr(m_Position) };
		returnMap["Scale"] = { PropertyType::FLOAT3, glm::value_ptr(m_Scale) };
		returnMap["Rotation"] = { PropertyType::FLOAT3, glm::value_ptr(m_Rotation) };

		return returnMap;
	}

private:
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
};