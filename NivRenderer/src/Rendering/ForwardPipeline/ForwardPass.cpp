#include "ForwardPass.h"

#include "Application/Util/Instrumentor.h"

ForwardPass::ForwardPass(Shader* passShader, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount) :
    RenderPass(passShader, resolutionWidth, resolutionHeight, sampleCount),
    m_ShadowmapShader(AssetManager::GetInstance().LoadShader("assets/shaders/shadowmap.glsl", ShaderType::VERTEX_AND_FRAGMENT))
{}

void ForwardPass::Run(Scene* scene, ProxyManager& proxyManager, CommandBuffer& commandBuffer)
{
    RendererState rendererState;

    rendererState.BoundUniformBuffers[0] = m_UniformBuffers["MatricesBlock"]->GetId();
    rendererState.BoundUniformBuffers[1] = m_UniformBuffers["LightBlock"]->GetId();
    rendererState.BoundUniformBuffers[2] = m_UniformBuffers["SettingsBlock"]->GetId();

    rendererState.Flags |= RendererStateFlag::DEPTH_TEST;
    rendererState.Flags |= RendererStateFlag::DEPTH_LESS;
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
    */
    {
        PROFILE_SCOPE("ForwardPass::RenderScene")
        // Render scene
        rendererState.Flags |= RendererStateFlag::CULL_FACE_BACK;
        rendererState.BoundWriteFramebuffer = m_OutputFramebuffer->GetId();
        rendererState.WriteFramebufferWidth = m_OutputFramebuffer->GetWidth();
        rendererState.WriteFramebufferHeight = m_OutputFramebuffer->GetHeight();
        const auto camera = dynamic_cast<CameraProxy*>(proxyManager.GetProxy(scene->GetCameraId()));

        commandBuffer.Submit({CommandType::CLEAR_COLOR_DEPTH_BUFFER, rendererState, 0});
        rendererState.BoundShader = m_PassShader->GetId();
        glm::mat4 view = camera->GetView();
        const glm::mat4 projection = camera->GetProjection();
        const glm::mat4 viewProj = projection * view;
        const glm::vec3 viewPos = camera->GetPosition();

        // Set Shadowmap uniforms
        const bool hasShadowMap = lightSpaceMatrix != glm::mat4(1.0f);

        m_UniformBuffers["MatricesBlock"]->BufferData(glm::value_ptr(viewProj), sizeof(glm::mat4), 1);
        m_UniformBuffers["MatricesBlock"]->BufferData(glm::value_ptr(lightSpaceMatrix), sizeof(glm::mat4), 2);
        if (hasShadowMap)
        {
            rendererState.BoundTextures[10] = {
                static_cast<int32_t>(m_ShadowmapFramebuffer->GetTextureAttachment()->GetTextureId()),
                                              m_PassShader->GetUniformLocation("shadowMap")};
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
                if (directionalLightProxy->GetDirtyFlag())
                {
                    m_UniformBuffers["LightBlock"]->BufferData(glm::value_ptr(directionalLightProxy->GetLightDirection()), sizeof(glm::vec3), 3);
                    m_UniformBuffers["LightBlock"]->BufferData(glm::value_ptr(directionalLightProxy->GetLightColor()), sizeof(glm::vec3), 4);
                    directionalLightProxy->GetDirtyFlag() = false;
                }
            }
            else if (pointLightProxy)
            {
                constexpr size_t pointLightBase = 5;
                const size_t pointLightOffset = 3 * pointLightIndex;
                uint32_t lightStrength = pointLightProxy->GetLightStrength();
                if (pointLightProxy->GetDirtyFlag())
                {
                    m_UniformBuffers["LightBlock"]->BufferData(glm::value_ptr(pointLightProxy->GetLightPosition()), sizeof(glm::vec3), pointLightBase + pointLightOffset);
                    m_UniformBuffers["LightBlock"]->BufferData(glm::value_ptr(pointLightProxy->GetLightColor()), sizeof(glm::vec3), pointLightBase + pointLightOffset + 1);
                    m_UniformBuffers["LightBlock"]->BufferData(&lightStrength, sizeof(uint32_t), pointLightBase + pointLightOffset + 2);
                    pointLightProxy->GetDirtyFlag() = false;
                }

                pointLightIndex++;
            }
        }
        m_UniformBuffers["LightBlock"]->BufferData(&hasDirectionalLight, sizeof(uint32_t), 0);
        m_UniformBuffers["LightBlock"]->BufferData(&pointLightIndex, sizeof(uint32_t), 1);
        m_UniformBuffers["LightBlock"]->BufferData(glm::value_ptr(viewPos), sizeof(glm::vec3), 2);

        for (const auto& sceneObjectMaterialProxy : proxyManager.GetSceneObjectsToRenderByMaterial(scene))
        {
            const auto materialProxy = dynamic_cast<MaterialProxy*>(proxyManager.GetProxy(sceneObjectMaterialProxy.first));

            rendererState.BoundTextures[0] = {static_cast<int32_t>((*materialProxy->GetDiffuseTexturePtr())->GetTextureId()),
                                              m_PassShader->GetUniformLocation("diffuseTexture")};

            if (materialProxy->HasNormalTexture())
            {
                rendererState.BoundTextures[1] = {
                    static_cast<int32_t>((*materialProxy->GetNormalTexturePtr())->GetTextureId()),
                                                  m_PassShader->GetUniformLocation("normalTexture")};
            }
            int setting = materialProxy->HasNormalTexture();
            m_UniformBuffers["SettingsBlock"]->BufferData(&setting, 4, 0);
            setting = hasShadowMap;
            m_UniformBuffers["SettingsBlock"]->BufferData(&setting, 4, 1);

            rendererState.BoundTextures[2] = {
                static_cast<int32_t>((*materialProxy->GetMetallicTexturePtr())->GetTextureId()),
                                              m_PassShader->GetUniformLocation("metallicTexture")};

            rendererState.BoundTextures[3] = {
                static_cast<int32_t>((*materialProxy->GetRoughnessTexturePtr())->GetTextureId()),
                                              m_PassShader->GetUniformLocation("roughnessTexture")};

            rendererState.BoundTextures[4] = {static_cast<int32_t>((*materialProxy->GetAOTexturePtr())->GetTextureId()),
                                              m_PassShader->GetUniformLocation("aoTexture")};

            rendererState.BoundTextures[5] = {
                static_cast<int32_t>((*materialProxy->GetEmissiveTexturePtr())->GetTextureId()),
                                              m_PassShader->GetUniformLocation("emissiveTexture")};

            for (const auto& sceneObjectProxy : sceneObjectMaterialProxy.second)
            {
                rendererState.BoundVertexArray = sceneObjectProxy->GetMeshProxy()->GetVertexArrayId();
                m_UniformBuffers["MatricesBlock"]->BufferData(glm::value_ptr(sceneObjectProxy->GetModelMatrix()), sizeof(glm::mat4), 0);

                const auto meshProxy = sceneObjectProxy->GetMeshProxy();

                if (meshProxy->GetIndexCount())
                    commandBuffer.Submit({CommandType::DRAW_INDEXED, rendererState, meshProxy->GetIndexCount()});
                else
                    commandBuffer.Submit({CommandType::DRAW, rendererState, meshProxy->GetVerticesCount()});
            }
        }

        if (scene->GetSceneSettings().visualizeLights && pointLightIndex)
        {
            const auto lightVisualizeShader = AssetManager::GetInstance().LoadShader(
                "assets/shaders/lightcube.glsl", ShaderType::VERTEX_AND_FRAGMENT);
            lightVisualizeShader->Bind();
            rendererState.BoundShader = lightVisualizeShader->GetId();

            rendererState.BoundVertexArray = LightProxy::GetVertexArrayId();
            LightProxy::Bind();
            for (const uint32_t id : scene->GetSceneLightIds())
            {
                const auto pointLightProxy = dynamic_cast<PointLightProxy*>(proxyManager.GetProxy(id));
                if (pointLightProxy)
                {
                    m_UniformBuffers["MatricesBlock"]->BufferData(glm::value_ptr(pointLightProxy->GetModelMatrix()),
                                                                  sizeof(glm::mat4), 0);
                    lightVisualizeShader->SetVec3("lightColor", pointLightProxy->GetLightColor());

                    commandBuffer.Submit({CommandType::DRAW, rendererState, LightProxy::GetVerticesCount()});
                    glDrawArrays(GL_TRIANGLES, 0, LightProxy::GetVerticesCount());
                }
            }
        }

        if (scene->HasSkybox())
        {
            const auto skyboxShader =
                AssetManager::GetInstance().LoadShader("assets/shaders/skybox.glsl", ShaderType::VERTEX_AND_FRAGMENT);
            const auto skyboxProxy = dynamic_cast<SkyboxProxy*>(proxyManager.GetProxy(scene->GetSkyboxObjectId()));

            rendererState.BoundShader = skyboxShader->GetId();

            if (skyboxProxy->HasAllTexturesSet())
            {
                skyboxShader->Bind();
                view = glm::mat4(glm::mat3(camera->GetView()));
                rendererState.BoundTextures[0] = {static_cast<int32_t>(skyboxProxy->GetTextureId()),
                    m_PassShader->GetUniformLocation("skybox")};

                rendererState.Flags &= ~RendererStateFlag::DEPTH_LESS;
                rendererState.Flags |= RendererStateFlag::DEPTH_LEQUAL;
                rendererState.BoundVertexArray = skyboxProxy->GetVertexArrayId();
                commandBuffer.Submit({CommandType::DRAW, rendererState, 36});
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
