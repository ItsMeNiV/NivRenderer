#pragma once
#include "Base.h"
#include "Entity/Entity.h"

class CameraObject : public Entity
{
public:
	CameraObject(uint32_t id) :
        Entity(id, std::string("Camera (") + std::to_string(id) + std::string(")")), m_CameraPtr(nullptr)
    {
    }

    ~CameraObject() override = default;

    std::vector<std::pair<std::string, Property>> GetEntityProperties() override
    {
        std::vector<std::pair<std::string, Property>> returnVector;
        return returnVector;
    }

	void SetCameraPtr(Camera* cameraPtr) { m_CameraPtr = cameraPtr; }
    Camera* GetCameraPtr() const { return m_CameraPtr; }

    nlohmann::ordered_json SerializeObject() { return {}; }
    void DeSerializeObject(nlohmann::json jsonObject) {}

private:
	Camera* m_CameraPtr;

};
