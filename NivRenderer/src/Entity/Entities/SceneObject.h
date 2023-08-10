#pragma once
#include "Base.h"
#include "Entity/Entity.h"

class SceneObject : public Entity
{
public:
	SceneObject(uint32_t id);

	virtual ~SceneObject();

	std::string *GetModelPath() { return &m_ModelPath; }
    void LoadMeshAndMaterial();

private:
    std::string m_ModelPath;

};