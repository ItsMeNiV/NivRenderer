#include "ForwardPass.h"

#include "Application/Util/Instrumentor.h"

ForwardPass::ForwardPass(Shader* passShader, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount) :
    RenderPass(passShader, resolutionWidth, resolutionHeight, sampleCount),
    m_ShadowmapShader(AssetManager::GetInstance().LoadShader("assets/shaders/shadowmap.glsl", ShaderType::VERTEX_AND_FRAGMENT))
{}

void ForwardPass::Run(Scene* scene, ProxyManager& proxyManager)
{
    glm::mat4 lightSpaceMatrix(1.0f);
    {
        PROFILE_SCOPE("ForwardPass::RenderShadowmap")

        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        // Render Shadowmap TODO: Allow multiple lights to be dynamic shadow casters - For now only Directional light
        for (const uint32_t sceneLightId : scene->GetSceneLightIds())
        {
            auto proxy = proxyManager.GetProxy(sceneLightId);
            const auto directionalLightProxy = dynamic_cast<DirectionalLightProxy*>(proxy);
            if (directionalLightProxy)
            {
                m_ShadowmapShader->Bind();

                glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);
                glm::vec3 lightPosition = -10.0f * directionalLightProxy->GetLightDirection();
                glm::vec3 up = directionalLightProxy->GetLightDirection() == glm::vec3(0.0f, -1.0f, 0.0f)
                    ? glm::vec3(1.0f, 0.0f, 0.0f)
                    : glm::vec3(0.0f, 1.0f, 0.0f);
                glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), up);
                lightSpaceMatrix = lightProjection * lightView;
                m_ShadowmapShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

                updateShadowmapFramebuffer(scene);
                m_ShadowmapFramebuffer->Bind();
                glViewport(0, 0, m_ShadowmapFramebuffer->GetWidth(), m_ShadowmapFramebuffer->GetHeight());
                glClear(GL_DEPTH_BUFFER_BIT);
                for (const auto& sceneObjectProxy : proxyManager.GetSceneObjectsToRender(scene))
                {
                    sceneObjectProxy->Bind();
                    m_ShadowmapShader->SetMat4("model", sceneObjectProxy->GetModelMatrix());
                    const auto& meshProxy = sceneObjectProxy->GetMeshProxy();
                    if (meshProxy->GetIndexCount())
                        glDrawElements(GL_TRIANGLES, meshProxy->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
                    else
                        glDrawArrays(GL_TRIANGLES, 0, meshProxy->GetVerticesCount());
                }
            }
        }
    }
    {
        PROFILE_SCOPE("ForwardPass::RenderScene")
        // Render scene
        glCullFace(GL_BACK);
        m_OutputFramebuffer->Bind();
        glViewport(0, 0, m_OutputFramebuffer->GetWidth(), m_OutputFramebuffer->GetHeight());
        const auto camera = dynamic_cast<CameraProxy*>(proxyManager.GetProxy(scene->GetCameraId()));

        glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_PassShader->Bind();
        glm::mat4 view = camera->GetView();
        glm::mat4 projection = camera->GetProjection();
        glm::mat4 viewProj = projection * view;
        m_PassShader->SetMat4("viewProjection", viewProj);
        const glm::vec3 viewPos = camera->GetPosition();
        m_PassShader->SetVec3("viewPos", viewPos);

        // Set Shadowmap uniforms
        const bool hasShadowMap = lightSpaceMatrix != glm::mat4(1.0f);
        m_PassShader->SetBool("hasShadowMap", hasShadowMap);
        m_PassShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
        if (hasShadowMap)
        {
            m_ShadowmapFramebuffer->GetTextureAttachment()->ActivateForSlot(10);
            m_PassShader->SetTexture("shadowMap", 10);
        }

        // Set Light uniforms
        uint32_t pointLightIndex = 0;
        bool hasDirectionalLight = false;
        for (const uint32_t id : scene->GetSceneLightIds())
        {
            const auto proxy = proxyManager.GetProxy(id);
            const auto directionalLightProxy = dynamic_cast<DirectionalLightProxy*>(proxy);
            const auto pointLightProxy = dynamic_cast<PointLightProxy*>(proxy);
            if (directionalLightProxy)
            {
                hasDirectionalLight = true;
                m_PassShader->SetVec3("directionalLight.color", directionalLightProxy->GetLightColor());
                m_PassShader->SetVec3("directionalLight.direction", directionalLightProxy->GetLightDirection());
            }
            else if (pointLightProxy)
            {
                m_PassShader->SetVec3("pointLights[" + std::to_string(pointLightIndex) + "].color",
                                      pointLightProxy->GetLightColor());
                m_PassShader->SetVec3("pointLights[" + std::to_string(pointLightIndex) + "].position",
                                      pointLightProxy->GetLightPosition());
                m_PassShader->SetInt("pointLights[" + std::to_string(pointLightIndex) + "].strength",
                                     pointLightProxy->GetLightStrength());
                pointLightIndex++;
            }
        }
        m_PassShader->SetBool("hasDirectionalLight", hasDirectionalLight);
        m_PassShader->SetInt("amountPointLights", pointLightIndex);

        for (const auto& sceneObjectProxy : proxyManager.GetSceneObjectsToRender(scene))
        {

            sceneObjectProxy->Bind();
            m_PassShader->SetMat4("model", sceneObjectProxy->GetModelMatrix());

            sceneObjectProxy->GetMaterialProxy()->BindDiffuseTexture(0);
            m_PassShader->SetTexture("diffuseTexture", 0);

            if (sceneObjectProxy->GetMaterialProxy()->HasNormalTexture())
            {
                sceneObjectProxy->GetMaterialProxy()->BindNormalTexture(1);
                m_PassShader->SetBool("hasNormalTexture", true);
                m_PassShader->SetTexture("normalTexture", 1);
            }
            else
            {
                m_PassShader->SetBool("hasNormalTexture", false);
            }

            sceneObjectProxy->GetMaterialProxy()->BindMetallicTexture(2);
            m_PassShader->SetTexture("metallicTexture", 2);

            sceneObjectProxy->GetMaterialProxy()->BindRoughnessTexture(3);
            m_PassShader->SetTexture("roughnessTexture", 3);

            sceneObjectProxy->GetMaterialProxy()->BindAOTexture(4);
            m_PassShader->SetTexture("aoTexture", 4);

            sceneObjectProxy->GetMaterialProxy()->BindEmissiveTexture(5);
            m_PassShader->SetTexture("emissiveTexture", 5);

            const auto meshProxy = sceneObjectProxy->GetMeshProxy();
            if (meshProxy->GetIndexCount())
                glDrawElements(GL_TRIANGLES, meshProxy->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
            else
                glDrawArrays(GL_TRIANGLES, 0, meshProxy->GetVerticesCount());
        }

        if (scene->GetSceneSettings().visualizeLights && pointLightIndex)
        {
            const auto lightVisualizeShader = AssetManager::GetInstance().LoadShader(
                "assets/shaders/lightcube.glsl", ShaderType::VERTEX_AND_FRAGMENT);
            lightVisualizeShader->Bind();
            lightVisualizeShader->SetMat4("viewProjection", viewProj);

            LightProxy::Bind();
            for (const uint32_t id : scene->GetSceneLightIds())
            {
                const auto pointLightProxy = dynamic_cast<PointLightProxy*>(proxyManager.GetProxy(id));
                if (pointLightProxy)
                {
                    lightVisualizeShader->SetVec3("lightColor", pointLightProxy->GetLightColor());
                    lightVisualizeShader->SetMat4("model", pointLightProxy->GetModelMatrix());

                    glDrawArrays(GL_TRIANGLES, 0, LightProxy::GetVerticesCount());
                }
            }
        }

        if (scene->HasSkybox())
        {
            const auto skyboxShader =
                AssetManager::GetInstance().LoadShader("assets/shaders/skybox.glsl", ShaderType::VERTEX_AND_FRAGMENT);
            const auto skyboxProxy = dynamic_cast<SkyboxProxy*>(proxyManager.GetProxy(scene->GetSkyboxObjectId()));
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
    }
    m_OutputFramebuffer->Unbind();
}

void ForwardPass::updateShadowmapFramebuffer(Scene* scene)
{
    if (!m_ShadowmapFramebuffer ||
        scene->GetSceneSettings().shadowmapResolution.x != m_ShadowmapFramebuffer->GetWidth() ||
        scene->GetSceneSettings().shadowmapResolution.y != m_ShadowmapFramebuffer->GetHeight())
    {
        m_ShadowmapFramebuffer = CreateRef<Framebuffer>(scene->GetSceneSettings().shadowmapResolution.x,
                                                        scene->GetSceneSettings().shadowmapResolution.y,
                                                        FramebufferAttachmentType::DEPTH_ONLY);
    }
}
