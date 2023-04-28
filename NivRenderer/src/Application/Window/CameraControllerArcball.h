#pragma once
#include "Base.h"

class CameraControllerArcball
{
public:
    CameraControllerArcball(Camera* camera, float distance, float scrollSensitivity);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessScroll(float yoffset);

private:
    float m_Distance, m_ScrollSensitivity;
    glm::vec4 m_Pivot;
    Camera* m_CameraPtr;

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    void updateCameraPos();

};