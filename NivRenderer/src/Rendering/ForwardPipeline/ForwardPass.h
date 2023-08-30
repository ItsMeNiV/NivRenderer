#pragma once
#include "Base.h"
#include "Application/Scene.h"
#include "Rendering/RenderPass.h"
#include "Rendering/Proxy/SceneObjectProxy.h"
#include "Rendering/Proxy/CameraProxy.h"
#include "Rendering/Proxy/LightProxy.h"
#include "Rendering/Proxy/SkyboxProxy.h"
#include "Rendering/Proxy/ProxyManager.h"

class ForwardPass : public RenderPass
{
public:
    ForwardPass(Ref<Shader> passShader, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount) :
        RenderPass(passShader, resolutionWidth, resolutionHeight, sampleCount)
    {}

    virtual void Run(Ref<Scene> scene, ProxyManager& proxyManager)
    {
        glEnable(GL_DEPTH_TEST);
        m_OutputFramebuffer->Bind();
        glViewport(0, 0, m_OutputFramebuffer->GetWidth(), m_OutputFramebuffer->GetHeight());
        Ref<CameraProxy> camera = std::static_pointer_cast<CameraProxy>(proxyManager.GetProxy(scene->GetCameraId()));

        glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_PassShader->Bind();
        glm::mat4 view = camera->GetView();
        glm::mat4 projection = camera->GetProjection();
        glm::mat4 viewProj = projection * view;
        m_PassShader->SetMat4("viewProjection", viewProj);
        glm::vec3 viewPos = camera->GetPosition();
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

            objectProxy->GetMaterialProxy()->BindDiffuseTexture(0);
            m_PassShader->SetTexture("diffuseTexture", 0);

            if (objectProxy->GetMaterialProxy()->HasNormalTexture())
            {
                objectProxy->GetMaterialProxy()->BindNormalTexture(1);
                m_PassShader->SetBool("hasNormalTexture", true);
                m_PassShader->SetTexture("normalTexture", 1);
            }
            else
            {
                m_PassShader->SetBool("hasNormalTexture", false);
            }

            objectProxy->GetMaterialProxy()->BindMetallicTexture(2);
            m_PassShader->SetTexture("metallicTexture", 2);

            objectProxy->GetMaterialProxy()->BindRoughnessTexture(3);
            m_PassShader->SetTexture("roughnessTexture", 3);

            objectProxy->GetMaterialProxy()->BindAOTexture(4);
            m_PassShader->SetTexture("aoTexture", 4);

            objectProxy->GetMaterialProxy()->BindEmissiveTexture(5);
            m_PassShader->SetTexture("emissiveTexture", 5);

            auto& meshProxy = objectProxy->GetMeshProxy();
            if (meshProxy->GetIndexCount())
                glDrawElements(GL_TRIANGLES, meshProxy->GetIndexCount(), GL_UNSIGNED_INT, 0);
            else
                glDrawArrays(GL_TRIANGLES, 0, meshProxy->GetVerticesCount());
        }

        if (scene->GetSceneSettings().visualizeLights && pointLightIndex)
        {
            Ref<Shader> lightVisualizeShader = AssetManager::GetInstance().LoadShader(std::string("assets/shaders/lightcube.glsl"),
                                                                                      ShaderType::VERTEX_AND_FRAGMENT);
            lightVisualizeShader->Bind();
            lightVisualizeShader->SetMat4("viewProjection", viewProj);

            LightProxy::Bind();
            for (uint32_t id : scene->GetSceneLightIds())
            {
                Ref<Proxy> proxy = proxyManager.GetProxy(id);
                bool isPointLight = std::dynamic_pointer_cast<PointLightProxy>(proxy) != 0;

                if (isPointLight)
                {
                    Ref<PointLightProxy> pointLightProxy =
                        std::static_pointer_cast<PointLightProxy>(proxyManager.GetProxy(id));
                    lightVisualizeShader->SetVec3("lightColor", pointLightProxy->GetLightColor());
                    lightVisualizeShader->SetMat4("model", pointLightProxy->GetModelMatrix());

                    glDrawArrays(GL_TRIANGLES, 0, LightProxy::GetVerticesCount());
                }
            }   
        }

        if (scene->HasSkybox())
        {
            Ref<Shader> skyboxShader = AssetManager::GetInstance().LoadShader(std::string("assets/shaders/skybox.glsl"),
                                                                              ShaderType::VERTEX_AND_FRAGMENT);
            Ref<SkyboxProxy> skyboxProxy =
                std::static_pointer_cast<SkyboxProxy>(proxyManager.GetProxy(scene->GetSkyboxObjectId()));
            if (skyboxProxy->HasAllTexturesSet())
            {
                skyboxShader->Bind();
                skyboxShader->SetMat4("projection", projection);
                view = glm::mat4(glm::mat3(camera->GetView()));
                skyboxShader->SetMat4("view", view);
                skyboxShader->SetTexture("skybox", 0);
                skyboxProxy->BindTexture(0);

                glDepthFunc(GL_LEQUAL);
                skyboxProxy->Bind();
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glDepthFunc(GL_LESS);   
            }
        }

        m_OutputFramebuffer->Unbind();
    }

private:

};
