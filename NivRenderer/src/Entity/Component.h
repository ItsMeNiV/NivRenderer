#pragma once

enum PropertyType
{
	FLOAT3, FLOAT, INT, STRING
};

struct ComponentProperty
{
	PropertyType type;
	void* valuePtr;
};

class Component
{
public:
	Component(const char* name) : m_Name(name) {}

	const char* GetName() const { return m_Name; }
	virtual std::unordered_map<std::string, ComponentProperty> GetComponentProperties() = 0;

private:
	const char* m_Name;
};