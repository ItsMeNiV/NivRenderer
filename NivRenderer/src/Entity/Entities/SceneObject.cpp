#include "SceneObject.h"

SceneObject::SceneObject(uint32_t id)
	: Entity(id, std::string("SceneObject (") + std::to_string(id) + std::string(")")), m_ModelPath("")
{
}

SceneObject::~SceneObject()
{
}

void SceneObject::LoadMeshAndMaterial()
{
}
