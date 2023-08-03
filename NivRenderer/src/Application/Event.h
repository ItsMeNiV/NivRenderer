#pragma once

enum EventCategory
{
	WindowCommandEventCategory
};

class Event
{
public:
	Event(EventCategory eventCategory) : m_EventCategory(eventCategory) {}
	virtual ~Event() = default;

	EventCategory GetEventCategory() { return m_EventCategory; }

private:
	EventCategory m_EventCategory;
};