#include "CameraControllerArcball.h"
#define _USE_MATH_DEFINES
#include <math.h>

CameraControllerArcball::CameraControllerArcball(Camera* camera, float distance, float scrollSensitivity)
    : m_CameraPtr(camera), m_Distance(distance), m_ScrollSensitivity(scrollSensitivity), m_Pivot({0.0f, 0.0f, 0.0f, 1.0f})
{
    glm::vec3 newPos = camera->GetPosition();
    newPos.z = distance;
    camera->SetPosition(newPos);
    camera->LookAt(m_Pivot);
}

void CameraControllerArcball::ProcessMouseMovement(float xoffset, float yoffset)
{
    glm::vec4 position(m_CameraPtr->GetPosition().x, m_CameraPtr->GetPosition().y, m_CameraPtr->GetPosition().z, 1);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    // step 1 : Calculate the amount of rotation given the mouse movement.
    float deltaAngleX = (2 * M_PI / m_CameraPtr->GetCameraWidth()); // a movement from left to right = 2*PI = 360 deg
    float deltaAngleY = (M_PI / m_CameraPtr->GetCameraHeight());  // a movement from top to bottom = PI = 180 deg
    float xAngle = -xoffset * deltaAngleX;
    float yAngle = yoffset * deltaAngleY;

    // Extra step to handle the problem when the camera direction is the same as the up vector
    float cosAngle = glm::dot(m_CameraPtr->GetCameraFront(), upVector);
    if (cosAngle * sgn(deltaAngleY) > 0.99f)
        deltaAngleY = 0;

    // step 2: Rotate the camera around the pivot point on the first axis.
    glm::mat4x4 rotationMatrixX(1.0f);
    rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, upVector);
    position = (rotationMatrixX * (position - m_Pivot)) + m_Pivot;

    // step 3: Rotate the camera around the pivot point on the second axis.
    glm::mat4x4 rotationMatrixY(1.0f);
    rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, m_CameraPtr->GetCameraRight());
    glm::vec3 finalPosition = (rotationMatrixY * (position - m_Pivot)) + m_Pivot;

    // Update the camera view (we keep the same lookat and the same up vector)
    m_CameraPtr->SetCameraView(finalPosition, m_Pivot, upVector);

    // Update the mouse position for the next rotation
    // app->m_lastMousePos.x = xPos;
    // app->m_lastMousePos.y = yPos;
}

void CameraControllerArcball::ProcessScroll(float yoffset)
{
    yoffset *= m_ScrollSensitivity;
    m_Distance = glm::clamp(m_Distance - yoffset, 1.0f, 50.0f);
    updateCameraPos();
}

void CameraControllerArcball::updateCameraPos()
{
    glm::vec3 newPos = m_CameraPtr->GetPosition();
    newPos = m_Distance * -m_CameraPtr->GetCameraFront();
    m_CameraPtr->SetPosition(newPos);
    m_CameraPtr->LookAt(m_Pivot);
}
