#pragma once
#include "Base.h"
#include "Rendering/OpenGL/Texture.h"

enum class FramebufferAttachmentType
{
    DEPTH_STENCIL_COLOR,
	DEPTH_ONLY
};

class Framebuffer
{
public:
    Framebuffer(const int width, const int height, const FramebufferAttachmentType attachmentType, const int sampleCount = 1);
	~Framebuffer();

	void Bind() const;
	void Unbind() const;

	void BlitFramebuffer(const unsigned int targetFramebuffer, const int targetWidth, const int targetHeight) const;

    Texture* GetTextureAttachment() { return m_TextureAttachment.get(); }
	int GetWidth() const { return m_CurrentWidth; }
	int GetHeight() const { return m_CurrentHeight; }
	unsigned int GetId() const { return m_FrameBuffer; }

private:
	unsigned int m_FrameBuffer, m_RenderBuffer;
	Scope<Texture> m_TextureAttachment;
	int m_CurrentWidth, m_CurrentHeight;
};