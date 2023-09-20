#include "ShaderAsset.h"

ShaderAsset::ShaderAsset(const std::string& path, ShaderType shaderType) :
    m_Shader(CreateScope<Shader>(path.c_str(), shaderType))
{}
