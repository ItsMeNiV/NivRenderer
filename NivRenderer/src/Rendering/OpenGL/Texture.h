#pragma once
#include "Base.h"

//TODO: Refactor - Use this class also in Proxies
enum TextureType
{
	TEXTURE_2D = GL_TEXTURE_2D,
	TEXTURE_2D_MULTI = GL_TEXTURE_2D_MULTISAMPLE,
	TEXTURE_CUBEMAP = GL_TEXTURE_CUBE_MAP
};
enum InternalFormat
{
    RGB = GL_RGB16,
	RGBA = GL_RGBA16,
	R = GL_R16,
    DEPTH = GL_DEPTH_COMPONENT32
};

class Texture
{
public:
	Texture(std::string&& path, bool flipVertically = false);
	Texture(std::vector<std::string> paths, bool flipVertically = false);
    Texture(const int width, const int height, const InternalFormat internalFormat = InternalFormat::RGB, const int sampleCount = 1);
	~Texture();

	void ActivateForSlot(uint32_t slot);

	unsigned int GetTextureId() { return m_TextureId; }
	unsigned int GetTextureType() { return m_TextureType; }

private:
	std::vector<std::string> m_Paths;
	unsigned int m_TextureId;
	TextureType m_TextureType;
};