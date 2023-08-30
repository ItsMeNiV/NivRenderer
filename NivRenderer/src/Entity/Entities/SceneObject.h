#pragma once
#include "Base.h"
#include "Entity/Entity.h"

class SceneObject : public Entity
{
public:
	SceneObject(uint32_t id);

	std::string *GetModelPath() { return &m_ModelPath; }
    void LoadMeshAndMaterial();

    std::vector<std::pair<std::string, EntityProperty>> GetEntityProperties() override;

private:
    std::string m_ModelPath;

};