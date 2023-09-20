#pragma once
#include "Base.h"

class ShaderAsset
{
public:
    ShaderAsset(const std::string& path, ShaderType shaderType);

    Shader* GetShader() const { return m_Shader.get(); }

private:
    Scope<Shader> m_Shader;
};