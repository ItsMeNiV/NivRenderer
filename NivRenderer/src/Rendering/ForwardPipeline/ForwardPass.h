#pragma once
#include "Base.h"
#include "Application/Scene.h"
#include "Rendering/RenderPass.h"
#include "Rendering/Proxy/SceneObjectProxy.h"
#include "Rendering/Proxy/CameraProxy.h"
#include "Rendering/Proxy/ProxyManager.h"

class ForwardPass : public RenderPass
{
public:
    ForwardPass(Ref<Shader> passShader, uint32_t resolutionWidth, uint32_t resolutionHeight)
        : RenderPass(passShader, resolutionWidth, resolutionHeight)
    {}

    virtual void Run(Ref<Scene> scene, ProxyManager& proxyManager)
    {
        m_OutputFramebuffer->Bind();
        Ref<CameraProxy> camera = std::static_pointer_cast<CameraProxy>(proxyManager.GetProxy(scene->GetCameraId()));

        //TEST
        glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        m_PassShader->Bind();
        glm::mat4 viewProj = camera->GetProjection() * camera->GetView();
        m_PassShader->SetMat4("viewProjection", viewProj);
        for (uint32_t id : scene->GetSceneObjectIds())
        {
            Ref<SceneObjectProxy> objectProxy = std::static_pointer_cast<SceneObjectProxy>(proxyManager.GetProxy(id));
            objectProxy->Bind();
            m_PassShader->SetMat4("model", objectProxy->GetModelMatrix());
            if (objectProxy->GetIndexCount())
                glDrawElements(GL_TRIANGLES, objectProxy->GetIndexCount(), GL_UNSIGNED_INT, 0);
            else
                glDrawArrays(GL_TRIANGLES, 0, objectProxy->GetVerticesCount());
        }


        m_OutputFramebuffer->Unbind();
    }

private:

};