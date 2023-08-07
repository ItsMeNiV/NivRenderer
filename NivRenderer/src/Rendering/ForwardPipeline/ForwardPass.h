#pragma once
#include "Base.h"
#include "Application/Scene.h"
#include "Rendering/RenderPass.h"
#include "Rendering/Proxy/SceneObjectProxy.h"
#include "Rendering/Proxy/CameraProxy.h"
#include "Rendering/Proxy/LightProxy.h"
#include "Rendering/Proxy/ProxyManager.h"

class ForwardPass : public RenderPass
{
public:
    ForwardPass(Ref<Shader> passShader, uint32_t resolutionWidth, uint32_t resolutionHeight)
        : RenderPass(passShader, resolutionWidth, resolutionHeight)
    {}

    virtual void Run(Ref<Scene> scene, ProxyManager& proxyManager)
    {
        glEnable(GL_DEPTH_TEST);
        m_OutputFramebuffer->Bind();
        Ref<CameraProxy> camera = std::static_pointer_cast<CameraProxy>(proxyManager.GetProxy(scene->GetCameraId()));

        glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_PassShader->Bind();
        glm::mat4 view = camera->GetView();
        glm::mat4 viewProj = camera->GetProjection() * view;
        m_PassShader->SetMat4("viewProjection", viewProj);
        glm::vec3 viewPos = glm::vec3(-view[3][0], -view[3][1], -view[3][2]);
        m_PassShader->SetVec3("viewPos", viewPos);

        //Set Light uniforms
        uint32_t pointLightIndex = 0;
        bool hasDirectionalLight = false;
        for (uint32_t id : scene->GetSceneLightIds())
        {
            Ref<Proxy> proxy = proxyManager.GetProxy(id);
            bool isDirectionalLight = std::dynamic_pointer_cast<DirectionalLightProxy>(proxy) != 0;
            bool isPointLight = std::dynamic_pointer_cast<PointLightProxy>(proxy) != 0;
            if (isDirectionalLight)
            {
                hasDirectionalLight = true;
                Ref<DirectionalLightProxy> directionalLightProxy =
                    std::static_pointer_cast<DirectionalLightProxy>(proxyManager.GetProxy(id));
                m_PassShader->SetVec3("directionalLight.color", directionalLightProxy->GetLightColor());
                m_PassShader->SetVec3("directionalLight.direction", directionalLightProxy->GetLightDirection());
            }
            else if (isPointLight)
            {
                Ref<PointLightProxy> pointLightProxy =
                    std::static_pointer_cast<PointLightProxy>(proxyManager.GetProxy(id));
                m_PassShader->SetVec3("pointLights[" + std::to_string(pointLightIndex) + "].color", pointLightProxy->GetLightColor());
                m_PassShader->SetVec3("pointLights[" + std::to_string(pointLightIndex) + "].position", pointLightProxy->GetLightPosition());
                m_PassShader->SetInt("pointLights[" + std::to_string(pointLightIndex) + "].strength", pointLightProxy->GetLightStrength());
                pointLightIndex++;
            }
        }
        m_PassShader->SetBool("hasDirectionalLight", hasDirectionalLight);
        m_PassShader->SetInt("amountPointLights", pointLightIndex);

        for (uint32_t id : scene->GetSceneObjectIds())
        {
            Ref<SceneObjectProxy> objectProxy = std::static_pointer_cast<SceneObjectProxy>(proxyManager.GetProxy(id));
            objectProxy->Bind();
            m_PassShader->SetMat4("model", objectProxy->GetModelMatrix());
            
            for (auto& t : objectProxy->GetTextures())
            {
                if (t.type == "texture_diffuse")
                {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, t.id);
                    m_PassShader->SetTexture("diffuseTexture", 0);
                }
            }
            

            if (objectProxy->GetIndexCount())
                glDrawElements(GL_TRIANGLES, objectProxy->GetIndexCount(), GL_UNSIGNED_INT, 0);
            else
                glDrawArrays(GL_TRIANGLES, 0, objectProxy->GetVerticesCount());
        }


        m_OutputFramebuffer->Unbind();
    }

private:

};