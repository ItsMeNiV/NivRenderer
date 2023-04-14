#pragma once
#include "Base.h"

enum PropertyType
{
	FLOAT3, FLOAT, INT, STRING, PATH
};

struct ComponentProperty
{
	PropertyType type;
	void* valuePtr;
	std::function<void()> callback;
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