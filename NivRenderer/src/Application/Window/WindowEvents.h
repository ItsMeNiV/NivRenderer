#pragma once
#include "Application/Event.h"

enum WindowCommand
{
    RecompileShaders
};

class WindowCommandEvent : public Event
{
public:
    WindowCommandEvent(WindowCommand command) : Event(EventCategory::WindowCommandEventCategory), m_WindowCommand(command) {}
    ~WindowCommandEvent() {};

    WindowCommand GetCommand() { return m_WindowCommand; }

private:
    WindowCommand m_WindowCommand;

};