#pragma once
#include "json.hpp"

using namespace nlohmann;

class Serializable
{
public:
    virtual ordered_json SerializeObject() = 0;
    virtual void DeSerializeObject() = 0;
};