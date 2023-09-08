#include "SceneObject.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/MaterialComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Application/Util/Math.h"

SceneObject::SceneObject(uint32_t id)
	: Entity(id, std::string("SceneObject (") + std::to_string(id) + std::string(")"))
{
}

void SceneObject::LoadModel()
{
    if (const auto meshComponent = ECSRegistry::GetInstance().GetComponent<MeshComponent>(GetId()))
    {
        ECSRegistry::GetInstance().RemoveComponent(GetId(), meshComponent->GetId());
    }
    if (const auto materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(GetId()))
    {
        ECSRegistry::GetInstance().RemoveComponent(GetId(), materialComponent->GetId());
    }
    if (!ECSRegistry::GetInstance().GetComponent<TransformComponent>(GetId()))
    {
        ECSRegistry::GetInstance().AddComponent<TransformComponent>(GetId());
    }

    const Model* model = AssetManager::GetInstance().LoadModel(m_ModelPath);
    for (auto& subModel : model->subModels)
        createChildSceneObjectFromSubModel(subModel, GetId());
}

std::vector<std::pair<std::string, Property>> SceneObject::GetEntityProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;

    returnVector.push_back({"Model Name", {PropertyType::STRING, &m_EntityName, [this](){}}});
    returnVector.push_back({"Model Path", {PropertyType::PATH, &m_ModelPath, [this]() { LoadModel(); }}});

    return returnVector;
}

void SceneObject::createChildSceneObjectFromSubModel(const SubModel& subModel, const uint32_t parentId)
{
    const auto subObject = ECSRegistry::GetInstance().CreateEntity<SceneObject>(parentId);
    auto transform = ECSRegistry::GetInstance().AddComponent<TransformComponent>( subObject->GetId());
    if (subModel.mesh)
    {
        const auto meshComponent = ECSRegistry::GetInstance().AddComponent<MeshComponent>(subObject->GetId());
        meshComponent->GetMeshAsset() = subModel.mesh;
        const std::string meshPath = subModel.mesh->GetPath();
        meshComponent->GetPath() = meshPath;
        *subObject->GetEntityName() = meshPath.substr(meshPath.find_last_of('/'), meshPath.size());
    }
    if (subModel.material)
    {
        const auto materialComponent = ECSRegistry::GetInstance().AddComponent<MaterialComponent>(subObject->GetId());
        materialComponent->GetMaterialAsset() = subModel.material;
    }
    Math::DecomposeMatrix(subModel.modelMatrix, transform->GetScale(), transform->GetRotation(),
                          transform->GetPosition());

    for (auto& nextSubModel : subModel.subModels)
        createChildSceneObjectFromSubModel(nextSubModel, subObject->GetId());
}
