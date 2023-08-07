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

uniform mat4 model;
uniform mat4 viewProjection;

void main()
{
    v_TextureCoords = vertTextureCoords;
    v_Normal = vertNormal;
    v_FragPos = vec3(model * vec4(vertPosition, 1.0));
    gl_Position = viewProjection * model * vec4(vertPosition.x, vertPosition.y, vertPosition.z, 1.0);
}

#endif

//=========================//

#ifdef FRAGMENT
#include "lighting.glsl"

#define MAX_POINT_LIGHTS 32

in vec2 v_TextureCoords;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform vec3 viewPos;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform bool hasDirectionalLight;
uniform int amountPointLights;

void main()
{
    vec3 normal = normalize(v_Normal);
    vec3 viewDir = normalize(viewPos - v_FragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);
    if(hasDirectionalLight)
        result += CalcDirLight(directionalLight, normal, viewDir, diffuseTexture, v_TextureCoords);

    for(int i = 0; i < amountPointLights; i++)
        result += CalcPointLight(pointLights[i], normal, viewDir, v_FragPos, diffuseTexture, v_TextureCoords);

    FragColor = vec4(result, 1.0);
}

#endif