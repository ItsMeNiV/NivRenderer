#include "Framebuffer.h"

Framebuffer::Framebuffer(const int width, const int height, const FramebufferAttachmentType attachmentType, const int sampleCount) :
    m_RenderBuffer(UINT_MAX),
    m_CurrentWidth(width), m_CurrentHeight(height)
{
    glGenFramebuffers(1, &m_FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

    if (attachmentType == FramebufferAttachmentType::DEPTH_STENCIL_COLOR)
    {
        m_TextureAttachment = CreateScope<Texture>(width, height, InternalFormat::RGB, sampleCount);
        glGenRenderbuffers(1, &m_RenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
        if (sampleCount > 1)
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, GL_DEPTH24_STENCIL8, width, height);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_TextureAttachment->GetTextureType(),
                               m_TextureAttachment->GetTextureId(), 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
    }
    else if (attachmentType == FramebufferAttachmentType::DEPTH_ONLY)
    {
        m_TextureAttachment = CreateScope<Texture>(width, height, InternalFormat::DEPTH);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TextureAttachment->GetTextureType(), m_TextureAttachment->GetTextureId(), 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        SPDLOG_ERROR(std::to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FrameBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFramebuffer);
    glBlitFramebuffer(0, 0, m_CurrentWidth, m_CurrentHeight, 0, 0, targetWidth, targetHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}