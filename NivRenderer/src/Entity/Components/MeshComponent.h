#pragma once
#include "Base.h"
#include "Entity/Component.h"

class MeshComponent : public Component
{
public:
	MeshComponent()
		: Component("MeshComponent")
	{}

	~MeshComponent()
	{}

	virtual std::unordered_map<std::string, ComponentProperty> GetComponentProperties()
	{
		std::unordered_map<std::string, ComponentProperty> returnMap;

		returnMap["Path"] = { PropertyType::STRING, &m_Path };

		return returnMap;
	}

private:
	std::string m_Path;

};