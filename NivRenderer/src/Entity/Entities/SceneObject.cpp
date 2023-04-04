#include "SceneObject.h"

SceneObject::SceneObject()
	: Entity(std::string("SceneObject (") + std::to_string(NextEntityId) + std::string(")"))
{
}

SceneObject::~SceneObject()
{
}
