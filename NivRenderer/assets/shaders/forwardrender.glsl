#version 330 core

#ifdef VERTEX

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertTextureCoords;
layout (location = 3) in vec3 vertTangent;
layout (location = 4) in vec3 vertBitangent;

out vec2 v_TextureCoords;
out vec3 v_Normal;
out vec3 v_FragPos;
out mat3 v_TBN;

uniform mat4 model;
uniform mat4 viewProjection;

void main()
{
    v_TextureCoords = vertTextureCoords;
    v_Normal = mat3(transpose(inverse(model))) * vertNormal;
    v_FragPos = vec3(model * vec4(vertPosition, 1.0));
    vec3 T = normalize(vec3(model * vec4(vertTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(vertBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(vertNormal, 0.0)));
    v_TBN = mat3(T, B, N);
    gl_Position = viewProjection * vec4(v_FragPos, 1.0);
}

#endif

//=========================//

#ifdef FRAGMENT
#include "lighting.glsl"

#define MAX_POINT_LIGHTS 32

in vec2 v_TextureCoords;
in vec3 v_Normal;
in vec3 v_FragPos;
in mat3 v_TBN;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
uniform vec3 viewPos;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform bool hasDirectionalLight;
uniform int amountPointLights;
uniform bool hasSpecularTexture;
uniform bool hasNormalTexture;

void main()
{
    vec3 normal;
    if(hasNormalTexture)
    {
        normal = texture(normalTexture, v_TextureCoords).rgb * 2.0 - 1.0;
        normal = normalize(v_TBN * normal);
    }
    else
    {
        normal = v_Normal;
    }
    vec3 viewDir = normalize(viewPos - v_FragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);
    if(hasDirectionalLight)
        result += CalcDirLight(directionalLight, normal, viewDir, diffuseTexture, hasSpecularTexture, specularTexture, v_TextureCoords);

    for(int i = 0; i < amountPointLights; i++)
        result += CalcPointLight(pointLights[i], normal, viewDir, v_FragPos, diffuseTexture, hasSpecularTexture, specularTexture, v_TextureCoords);

    FragColor = vec4(result, 1.0);
}

#endif