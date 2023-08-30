#pragma once
#include "Base.h"
#include "PropertyType.h"

struct ComponentProperty
{
    NivRenderer::PropertyType type;
	void* valuePtr;
	std::function<void()> callback;
};

class Component
{
public:
    Component(const uint32_t id, const char* name) : m_Id(id), m_Name(name) {}
    virtual ~Component() = default;

	const char* GetName() const { return m_Name; }
    const uint32_t& GetId() const { return m_Id; }
	virtual std::vector<std::pair<std::string, ComponentProperty>> GetComponentProperties() = 0;

private:
    uint32_t m_Id;
	const char* m_Name;
};