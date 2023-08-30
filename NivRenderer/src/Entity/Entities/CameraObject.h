#pragma once
#include "Base.h"
#include "Entity/Entity.h"

class CameraObject : public Entity
{
public:
	CameraObject(uint32_t id)
		: Entity(id, std::string("Camera (") + std::to_string(id) + std::string(")"))
	{
	}

    ~CameraObject() override = default;

    std::vector<std::pair<std::string, EntityProperty>> GetEntityProperties() override
    {
        std::vector<std::pair<std::string, EntityProperty>> returnVector;
        return returnVector;
    }

	void SetCameraPtr(const Ref<Camera> cameraPtr) { m_CameraPtr = cameraPtr; }
	const Ref<Camera>& GetCameraPtr() const { return m_CameraPtr; }
    

private:
	Ref<Camera> m_CameraPtr;

};