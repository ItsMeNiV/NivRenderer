#pragma once

class Component
{
public:
	Component(const char* name) : m_Name(name) {}

	const char* GetName() const { return m_Name; }

private:
	const char* m_Name;
};