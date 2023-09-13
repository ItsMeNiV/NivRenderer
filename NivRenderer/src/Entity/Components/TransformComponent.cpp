#include "TransformComponent.h"

TransformComponent::TransformComponent(const uint32_t id)
    : Component(id, "TransformComponent"),
    m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f),
    m_DegRotation(0.0f, 0.0f, 0.0f)
{}

TransformComponent::~TransformComponent() = default;

std::vector<std::pair<std::string, Property>> TransformComponent::GetComponentProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;

    returnVector.push_back({"Position", {PropertyType::FLOAT3, glm::value_ptr(m_Position), []() {}}});
    returnVector.push_back({"Scale", {PropertyType::FLOAT3, glm::value_ptr(m_Scale), []() {}}});
    m_DegRotation = glm::degrees(m_Rotation);
    returnVector.push_back({"Rotation", {PropertyType::FLOAT3, glm::value_ptr(m_DegRotation), [this]() {
                                             m_Rotation = glm::radians(m_DegRotation);
                                         }}});

    return returnVector;
}

glm::vec3& TransformComponent::GetPosition() { return m_Position; }

glm::vec3& TransformComponent::GetScale() { return m_Scale; }

glm::vec3& TransformComponent::GetRotation() { return m_Rotation; }

ordered_json TransformComponent::SerializeObject()
{
    ordered_json component = {
        {"Id", GetId()},
        {"Type", "TransformComponent"},
        {"Name", GetName()},
        {"Position", {{"x", m_Position.x}, {"y", m_Position.y}, {"z", m_Position.z}}},
        {"Scale", {{"x", m_Scale.x}, {"y", m_Scale.y}, {"z", m_Scale.z}}},
        {"Rotation", {{"x", m_Rotation.x}, {"y", m_Rotation.y}, {"z", m_Rotation.z}}},
        {"DegRotation", {{"x", m_DegRotation.x}, {"y", m_DegRotation.y}, {"z", m_DegRotation.z}}},
    };

    return component;
}

void TransformComponent::DeSerializeObject(json jsonObject)
{
    m_Position = {jsonObject["Position"]["x"], jsonObject["Position"]["y"], jsonObject["Position"]["z"]};
    m_Scale = {jsonObject["Scale"]["x"], jsonObject["Scale"]["y"], jsonObject["Scale"]["z"]};
    m_Rotation = {jsonObject["Rotation"]["x"], jsonObject["Rotation"]["y"], jsonObject["Rotation"]["z"]};
    m_DegRotation = {jsonObject["DegRotation"]["x"], jsonObject["DegRotation"]["y"], jsonObject["DegRotation"]["z"]};
}


