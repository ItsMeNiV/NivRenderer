#pragma once
#include "Base.h"
#include "Entity/Entity.h"

class LightObject : public Entity
{
public:
	LightObject(uint32_t id, std::string&& name) : Entity(id, std::move(name)),
		m_LightColor(1.0, 1.0, 1.0)
	{}

	void SetLightColor(glm::vec3& color) { m_LightColor = color; }
	glm::vec3& GetLightColor() { return m_LightColor; }

private:
	glm::vec3 m_LightColor;
};

class DirectionalLightObject : public LightObject
{
public:
	DirectionalLightObject(uint32_t id) : LightObject(id, std::string("DirectionalLight (") + std::to_string(id) + std::string(")")),
		m_Direction(0.0, -1.0, 0.0)
	{}

	~DirectionalLightObject() = default;

	void SetDirection(glm::vec3& direction) { m_Direction = direction; }
	glm::vec3& GetDirection() { return m_Direction; }

private:
	glm::vec3 m_Direction;
};