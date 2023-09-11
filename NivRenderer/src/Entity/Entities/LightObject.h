#pragma once
#include "Base.h"
#include "Entity/Entity.h"

class LightObject : public Entity
{
public:
	LightObject(uint32_t id, std::string&& name) : Entity(id, std::move(name)),
		m_LightColor(1.0f, 1.0f, 1.0f)
	{}

	void SetLightColor(glm::vec3& color) { m_LightColor = color; }
	glm::vec3& GetLightColor() { return m_LightColor; }

protected:
	glm::vec3 m_LightColor;
};

class DirectionalLightObject : public LightObject
{
public:
	DirectionalLightObject(uint32_t id) : LightObject(id, std::string("Directional Light")),
		m_Direction(0.0f, -1.0f, 0.0f)
	{}

	~DirectionalLightObject() = default;

	void SetDirection(glm::vec3& direction) { m_Direction = direction; }
	glm::vec3& GetDirection() { return m_Direction; }

	std::vector<std::pair<std::string, Property>> GetEntityProperties() override
    {
        std::vector<std::pair<std::string, Property>> returnVector;
        returnVector.push_back({"Light Color",{PropertyType::COLOR, glm::value_ptr(m_LightColor), [this]() {}}});
        returnVector.push_back({"Light Direction",{PropertyType::FLOAT3, glm::value_ptr(m_Direction), [this]() {}}});

        return returnVector;
    }

	ordered_json SerializeObject() override
	{
        ordered_json object = {
			{"Id", m_EntityId},
            {"Name", m_EntityName},
			{"Type", "DirectionalLight"},
            {"LightColor", {{"r", m_LightColor.r}, {"g", m_LightColor.g}, {"b", m_LightColor.b}}},
            {"Direction", {{"x", m_Direction.x}, {"y", m_Direction.y}, {"z", m_Direction.z}}}
        };

	    return object;
	}

private:
	glm::vec3 m_Direction;
};

class PointLightObject : public LightObject
{
public:
	PointLightObject(uint32_t id) : LightObject(id, std::string("Point Light (") + std::to_string(id) + std::string(")")),
		m_Position(0.0f, 0.0f, 0.0f), m_Strength(50)
	{}

	~PointLightObject() = default;

	void SetPosition(glm::vec3& position) { m_Position = position; }
	glm::vec3& GetPosition() { return m_Position; }
	void SetStrength(uint32_t strength) { m_Strength = strength; }
	uint32_t& GetStrength() { return m_Strength; }

	std::vector<std::pair<std::string, Property>> GetEntityProperties() override
    {
        std::vector<std::pair<std::string, Property>> returnVector;
        returnVector.push_back({"Light Color", {PropertyType::COLOR, glm::value_ptr(m_LightColor), [this]() {}}});
        returnVector.push_back({"Light Position", {PropertyType::FLOAT3, glm::value_ptr(m_Position), [this]() {}}});
        returnVector.push_back({"Light Strength", {PropertyType::SLIDER, &m_Strength, [this]() {}}});
        return returnVector;
    }

	ordered_json SerializeObject() override
    {
        ordered_json object = {
            {"Id", m_EntityId},
            {"Name", m_EntityName},
            {"Type", "PointLight"},
            {"LightColor", {{"r", m_LightColor.r}, {"g", m_LightColor.g}, {"b", m_LightColor.b}}},
            {"Position", {{"x", m_Position.x}, {"y", m_Position.y}, {"z", m_Position.z}}},
            {"Strength", m_Strength}
        };

        return object;
    }

private:
	glm::vec3 m_Position;
	uint32_t m_Strength;
};