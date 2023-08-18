#pragma once
#include "Base.h"
#include "Rendering/OpenGL/Texture.h"

//TODO: Refactor
class Framebuffer
{
public:
	Framebuffer(int width, int height, int sampleCount = 1);
	~Framebuffer();

	void Bind();
	void Unbind();

	void BlitFramebuffer(unsigned int targetFramebuffer, int targetWidth, int targetHeight);

	const Ref<Texture> GetTextureColorBuffer() { return m_TextureColorBuffer; }
	const int GetWidth() { return m_CurrentWidth; }
	const int GetHeight() { return m_CurrentHeight; }
	const unsigned int GetId() { return m_FrameBuffer; }

private:
	unsigned int m_FrameBuffer, m_RenderBuffer;
	Ref<Texture> m_TextureColorBuffer;
	int m_CurrentWidth, m_CurrentHeight;
};