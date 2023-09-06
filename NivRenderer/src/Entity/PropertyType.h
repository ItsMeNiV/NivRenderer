#pragma once

enum class PropertyType
{
    FLOAT3,
    FLOAT2,
    FLOAT,
    COLOR,
    INT,
    INT2,
    SLIDER,
    STRING,
    PATH,
    BOOL,
    SEPARATORTEXT,
    BUTTON,
    MATERIALDROPDOWN
};

struct Property
{
    PropertyType type;
    void* valuePtr;
    std::function<void()> callback;
};