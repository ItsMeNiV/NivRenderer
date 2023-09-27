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

SceneObject::~SceneObject() = default;

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

    const auto model = AssetManager::GetInstance().LoadModel(m_ModelPath);
    m_EntityName = model->name;
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

ordered_json SceneObject::SerializeObject()
{
    ordered_json object = {
        {"Id", m_EntityId},
        {"Name", m_EntityName},
        {"ModelPath", m_ModelPath}
    };

    const auto components = ECSRegistry::GetInstance().GetAllComponents(m_EntityId);
    if(!components.empty())
    {
        object["Components"] = json::array();
        uint32_t i = 0;
        for (const auto& component : components)
        {
            object["Components"][i] = component->SerializeObject();
            i++;
        } 
    }

    const auto& childEntities = GetChildEntities();
    if (!childEntities.empty())
    {
        object["ChildEntities"] = json::array();
        uint32_t i = 0;
        for (auto& child : GetChildEntities())
        {
            object["ChildEntities"][i] = child->SerializeObject();
            i++;
        }   
    }

    return object;
}

void SceneObject::DeSerializeObject(json jsonObject)
{
    m_EntityName = jsonObject["Name"];
    m_ModelPath = jsonObject["ModelPath"];

    if (jsonObject.contains("Components"))
    {
        for (json component : jsonObject["Components"])
        {
            IdManager::GetInstance().SetNextId(component["Id"]);
            if (component["Type"] == "TransformComponent")
            {
                const auto transformComponent = ECSRegistry::GetInstance().AddComponent<TransformComponent>(GetId());
                transformComponent->DeSerializeObject(component);
            }
            else if (component["Type"] == "MeshComponent")
            {
                const auto meshComponent = ECSRegistry::GetInstance().AddComponent<MeshComponent>(GetId());
                meshComponent->DeSerializeObject(component);
            }
            else if(component["Type"] == "MaterialComponent")
            {
                const auto materialComponent = ECSRegistry::GetInstance().AddComponent<MaterialComponent>(GetId());
                materialComponent->DeSerializeObject(component);
            }
        }   
    }

    if (jsonObject.contains("ChildEntities"))
    {
        for (json childEntity : jsonObject["ChildEntities"])
        {
            IdManager::GetInstance().SetNextId(childEntity["Id"]);
            auto childObject = ECSRegistry::GetInstance().CreateEntity<SceneObject>(GetId());
            childObject->DeSerializeObject(childEntity);
            AddChildEntity(childObject);
        }   
    }
}

void SceneObject::createChildSceneObjectFromSubModel(const SubModel& subModel, const uint32_t parentId)
{
    if (!subModel.mesh && subModel.subModels.empty())
        return; // Empty node, not interesting for us

    const auto subObject = ECSRegistry::GetInstance().CreateEntity<SceneObject>(parentId);
    const auto transform = ECSRegistry::GetInstance().AddComponent<TransformComponent>(subObject->GetId());

    if (subModel.mesh)
    {
        const auto meshComponent = ECSRegistry::GetInstance().AddComponent<MeshComponent>(subObject->GetId());
        meshComponent->SetMeshAsset(subModel.mesh);
        const std::string meshPath = subModel.mesh->GetPath();
        meshComponent->GetPath() = meshPath;
    }
    if (subModel.material)
    {
        const auto materialComponent = ECSRegistry::GetInstance().AddComponent<MaterialComponent>(subObject->GetId());
        materialComponent->SetMaterialAsset(subModel.material);
    }
    *subObject->GetEntityName() = subModel.name;

    Math::DecomposeMatrix(glm::value_ptr(subModel.modelMatrix), glm::value_ptr(transform->GetScale()),
                          glm::value_ptr(transform->GetRotation()), glm::value_ptr(transform->GetPosition()));

    for (auto& nextSubModel : subModel.subModels)
        createChildSceneObjectFromSubModel(nextSubModel, subObject->GetId());
}
