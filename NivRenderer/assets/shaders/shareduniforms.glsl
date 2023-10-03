uniform mat4 model;

layout (std140, binding = 0) uniform MatricesBlock
{
    mat4 viewProjection;
    mat4 lightSpaceMatrix;
};

layout (std140, binding = 2) uniform SettingsBlock
{
    bool hasNormalTexture;
    bool hasShadowMap;
};

#ifdef LIGHTING_USED
layout (std140, binding = 1) uniform LightBlock
{
    bool hasDirectionalLight;
    int amountPointLights;
    vec3 viewPos;
    DirectionalLight directionalLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
};
#endif