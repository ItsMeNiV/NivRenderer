#include "Framebuffer.h"

Framebuffer::Framebuffer(const int width, const int height, const FramebufferAttachmentType attachmentType, const int sampleCount) :
    m_RenderBuffer(UINT_MAX),
    m_CurrentWidth(width), m_CurrentHeight(height)
{
    glCreateFramebuffers(1, &m_FrameBuffer);

    if (attachmentType == FramebufferAttachmentType::DEPTH_STENCIL_COLOR)
    {
        m_TextureAttachment = CreateScope<Texture>(width, height, InternalFormat::RGB, sampleCount);

        glCreateRenderbuffers(1, &m_RenderBuffer);

        if (sampleCount > 1)
            glNamedRenderbufferStorageMultisample(m_RenderBuffer, sampleCount, GL_DEPTH24_STENCIL8, width, height);
        else
            glNamedRenderbufferStorage(m_RenderBuffer, GL_DEPTH24_STENCIL8, width, height);

        glNamedFramebufferTexture(m_FrameBuffer, GL_COLOR_ATTACHMENT0, m_TextureAttachment->GetTextureId(), 0);
        glNamedFramebufferRenderbuffer(m_FrameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
    }
    else if (attachmentType == FramebufferAttachmentType::DEPTH_ONLY)
    {
        m_TextureAttachment = CreateScope<Texture>(width, height, InternalFormat::DEPTH);
        glNamedFramebufferTexture(m_FrameBuffer, GL_DEPTH_ATTACHMENT, m_TextureAttachment->GetTextureId(), 0);
        glNamedFramebufferDrawBuffer(m_FrameBuffer, GL_NONE);
        glNamedFramebufferReadBuffer(m_FrameBuffer, GL_NONE);
    }

    if (glCheckNamedFramebufferStatus(m_FrameBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        SPDLOG_ERROR(std::to_string(glCheckNamedFramebufferStatus(m_FrameBuffer, GL_FRAMEBUFFER)));
    }
}

Framebuffer::~Framebuffer()
{
    glDeleteRenderbuffers(1, &m_RenderBuffer);
    glDeleteFramebuffers(1, &m_FrameBuffer);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::BlitFramebuffer(const unsigned int targetFramebuffer, const int targetWidth, const int targetHeight) const
{
    glBlitNamedFramebuffer(m_FrameBuffer, targetFramebuffer, 0, 0, m_CurrentWidth, m_CurrentHeight, 0, 0, targetWidth,
                           targetHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}