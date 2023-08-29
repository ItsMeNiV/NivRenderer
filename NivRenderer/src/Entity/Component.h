#pragma once
#include "Base.h"

namespace NivRenderer
{
    enum PropertyType
    {
        FLOAT3,
        FLOAT,
        INT,
        STRING,
        PATH,
        BOOL
    };
}

struct ComponentProperty
{
    NivRenderer::PropertyType type;
	void* valuePtr;
	std::function<void()> callback;
};

class Component
{
public:
    Component(const char* name) : m_Name(name) {}
    virtual ~Component() = default;

	const char* GetName() const { return m_Name; }
	virtual std::vector<std::pair<std::string, ComponentProperty>> GetComponentProperties() = 0;

private:
	const char* m_Name;
};