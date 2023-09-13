#pragma once
#include "Base.h"
#include "Entity/Entity.h"
#include "Entity/Assets/AssetManager.h"

class SceneObject : public Entity
{
public:
    ~SceneObject() override;

	SceneObject(uint32_t id);

	std::string *GetModelPath() { return &m_ModelPath; }
    void LoadModel();

    std::vector<std::pair<std::string, Property>> GetEntityProperties() override;

    ordered_json SerializeObject() override;
    void DeSerializeObject(json jsonObject) override;

private:
    std::string m_ModelPath;

    static void createChildSceneObjectFromSubModel(const SubModel& subModel, const uint32_t parentId);

};