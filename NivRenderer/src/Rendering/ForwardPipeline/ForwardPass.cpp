#include "ForwardPass.h"

#include "Application/Util/Instrumentor.h"

ForwardPass::ForwardPass(ShaderAsset* passShader, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount) :
    RenderPass(passShader, resolutionWidth, resolutionHeight, sampleCount),
    m_ShadowmapShader(AssetManager::GetInstance().LoadShader("assets/shaders/shadowmap.glsl", ShaderType::VERTEX_AND_FRAGMENT))
{}

void ForwardPass::Run(Scene* scene, ProxyManager& proxyManager)
{
    glEnable(GL_DEPTH_TEST);
    glm::mat4 lightSpaceMatrix(1.0f);
    /*
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
                m_ShadowmapShader->GetShader()->Bind();

                glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);
                glm::vec3 lightPosition = -10.0f * directionalLightProxy->GetLightDirection();
                glm::vec3 up = directionalLightProxy->GetLightDirection() == glm::vec3(0.0f, -1.0f, 0.0f)
                    ? glm::vec3(1.0f, 0.0f, 0.0f)
                    : glm::vec3(0.0f, 1.0f, 0.0f);
                glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), up);
                lightSpaceMatrix = lightProjection * lightView;
                m_ShadowmapShader->GetShader()->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

                updateShadowmapFramebuffer(scene);
                m_ShadowmapFramebuffer->Bind();
                glViewport(0, 0, m_ShadowmapFramebuffer->GetWidth(), m_ShadowmapFramebuffer->GetHeight());
                glClear(GL_DEPTH_BUFFER_BIT);
                for (const auto& sceneObjectProxy : proxyManager.GetSceneObjectsToRender(scene))
                {
                    sceneObjectProxy->Bind();
                    m_ShadowmapShader->GetShader()->SetMat4("model", sceneObjectProxy->GetModelMatrix());
                    const auto& meshProxy = sceneObjectProxy->GetMeshProxy();
                    if (meshProxy->GetIndexCount())
                        glDrawElements(GL_TRIANGLES, meshProxy->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
                    else
                        glDrawArrays(GL_TRIANGLES, 0, meshProxy->GetVerticesCount());
                }
            }
        }
    }
    */
    {
        PROFILE_SCOPE("ForwardPass::RenderScene")
        // Render scene
        glCullFace(GL_BACK);
        m_OutputFramebuffer->Bind();
        glViewport(0, 0, m_OutputFramebuffer->GetWidth(), m_OutputFramebuffer->GetHeight());
        const auto camera = dynamic_cast<CameraProxy*>(proxyManager.GetProxy(scene->GetCameraId()));

        glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_PassShader->GetShader()->Bind();
        glm::mat4 view = camera->GetView();
        glm::mat4 projection = camera->GetProjection();
        glm::mat4 viewProj = projection * view;
        m_PassShader->GetShader()->SetMat4("viewProjection", viewProj);
        const glm::vec3 viewPos = camera->GetPosition();
        m_PassShader->GetShader()->SetVec3("viewPos", viewPos);

        // Set Shadowmap uniforms
        const bool hasShadowMap = lightSpaceMatrix != glm::mat4(1.0f);
        m_PassShader->GetShader()->SetBool("hasShadowMap", hasShadowMap);
        m_PassShader->GetShader()->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
        if (hasShadowMap)
        {
            m_ShadowmapFramebuffer->GetTextureAttachment()->ActivateForSlot(10);
            m_PassShader->GetShader()->SetTexture("shadowMap", 10);
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
                m_PassShader->GetShader()->SetVec3("directionalLight.color", directionalLightProxy->GetLightColor());
                m_PassShader->GetShader()->SetVec3("directionalLight.direction",
                                                   directionalLightProxy->GetLightDirection());
            }
            else if (pointLightProxy)
            {
                m_PassShader->GetShader()->SetVec3("pointLights[" + std::to_string(pointLightIndex) + "].color",
                                      pointLightProxy->GetLightColor());
                m_PassShader->GetShader()->SetVec3("pointLights[" + std::to_string(pointLightIndex) + "].position",
                                      pointLightProxy->GetLightPosition());
                m_PassShader->GetShader()->SetInt("pointLights[" + std::to_string(pointLightIndex) + "].strength",
                                     pointLightProxy->GetLightStrength());
                pointLightIndex++;
            }
        }
        m_PassShader->GetShader()->SetBool("hasDirectionalLight", hasDirectionalLight);
        m_PassShader->GetShader()->SetInt("amountPointLights", pointLightIndex);

        for (const auto& sceneObjectMaterialProxy : proxyManager.GetSceneObjectsToRenderByMaterial(scene))
        {
            const auto materialProxy = dynamic_cast<MaterialProxy*>(proxyManager.GetProxy(sceneObjectMaterialProxy.first));
            materialProxy->BindDiffuseTexture(0);
            m_PassShader->GetShader()->SetTexture("diffuseTexture", 0);

            if (materialProxy->HasNormalTexture())
            {
                materialProxy->BindNormalTexture(1);
                m_PassShader->GetShader()->SetBool("hasNormalTexture", true);
                m_PassShader->GetShader()->SetTexture("normalTexture", 1);
            }
            else
            {
                m_PassShader->GetShader()->SetBool("hasNormalTexture", false);
            }

            materialProxy->BindMetallicTexture(2);
            m_PassShader->GetShader()->SetTexture("metallicTexture", 2);

            materialProxy->BindRoughnessTexture(3);
            m_PassShader->GetShader()->SetTexture("roughnessTexture", 3);

            materialProxy->BindAOTexture(4);
            m_PassShader->GetShader()->SetTexture("aoTexture", 4);

            materialProxy->BindEmissiveTexture(5);
            m_PassShader->GetShader()->SetTexture("emissiveTexture", 5);

            for (const auto& sceneObjectProxy : sceneObjectMaterialProxy.second)
            {
                sceneObjectProxy->Bind();
                m_PassShader->GetShader()->SetMat4("model", sceneObjectProxy->GetModelMatrix());

                const auto meshProxy = sceneObjectProxy->GetMeshProxy();
                if (meshProxy->GetIndexCount())
                    glDrawElements(GL_TRIANGLES, meshProxy->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
                else
                    glDrawArrays(GL_TRIANGLES, 0, meshProxy->GetVerticesCount());
            }
        }

        if (scene->GetSceneSettings().visualizeLights && pointLightIndex)
        {
            const auto lightVisualizeShader = AssetManager::GetInstance().LoadShader(
                "assets/shaders/lightcube.glsl", ShaderType::VERTEX_AND_FRAGMENT);
            lightVisualizeShader->GetShader()->Bind();
            lightVisualizeShader->GetShader()->SetMat4("viewProjection", viewProj);

            LightProxy::Bind();
            for (const uint32_t id : scene->GetSceneLightIds())
            {
                const auto pointLightProxy = dynamic_cast<PointLightProxy*>(proxyManager.GetProxy(id));
                if (pointLightProxy)
                {
                    lightVisualizeShader->GetShader()->SetVec3("lightColor", pointLightProxy->GetLightColor());
                    lightVisualizeShader->GetShader()->SetMat4("model", pointLightProxy->GetModelMatrix());

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
                skyboxShader->GetShader()->Bind();
                skyboxShader->GetShader()->SetMat4("projection", projection);
                view = glm::mat4(glm::mat3(camera->GetView()));
                skyboxShader->GetShader()->SetMat4("view", view);
                skyboxShader->GetShader()->SetTexture("skybox", 0);
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
        m_ShadowmapFramebuffer = CreateScope<Framebuffer>(scene->GetSceneSettings().shadowmapResolution.x,
                                                        scene->GetSceneSettings().shadowmapResolution.y,
                                                        FramebufferAttachmentType::DEPTH_ONLY);
    }
}
