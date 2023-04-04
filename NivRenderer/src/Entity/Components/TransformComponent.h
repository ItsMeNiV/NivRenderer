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

private:
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
};