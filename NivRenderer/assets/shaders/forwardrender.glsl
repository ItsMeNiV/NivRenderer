#version 460 core

#define USE_PBR

#ifdef VERTEX

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertTextureCoords;
layout (location = 3) in vec3 vertTangent;
layout (location = 4) in vec3 vertBitangent;

out vec2 v_TextureCoords;
out vec3 v_Normal;
out vec3 v_FragPos;
out vec4 v_FragPosLightSpace;
out mat3 v_TBN;

#include "shareduniforms.glsl"

void main()
{
    v_TextureCoords = vertTextureCoords;
    v_Normal = mat3(transpose(inverse(model))) * vertNormal;
    v_FragPos = vec3(model * vec4(vertPosition, 1.0));
    v_FragPosLightSpace = lightSpaceMatrix * vec4(v_FragPos, 1.0);
    vec3 T = normalize(vec3(model * vec4(vertTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(vertBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(vertNormal, 0.0)));
    v_TBN = mat3(T, B, N);
    gl_Position = viewProjection * vec4(v_FragPos, 1.0);
}

#endif

//=========================//

#ifdef FRAGMENT

const float PI = 3.14159265359;

#include "lighting.glsl"

#define MAX_POINT_LIGHTS 32

in vec2 v_TextureCoords;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec4 v_FragPosLightSpace;
in mat3 v_TBN;

out vec4 FragColor;

// ============= Material =============
uniform sampler2D diffuseTexture;//   |
uniform sampler2D normalTexture;//    |
uniform sampler2D metallicTexture;//  |
uniform sampler2D roughnessTexture;// |
uniform sampler2D aoTexture;//        |
uniform sampler2D emissiveTexture;//  |
// ============= Material =============

uniform sampler2D shadowMap;

#include "shareduniforms.glsl"

bool calculateShadow(vec4 fragPosLightSpace);

void main()
{
    vec3 N;
    if(hasNormalTexture)
    {
        N = texture(normalTexture, v_TextureCoords).rgb * 2.0 - 1.0;
        N = normalize(v_TBN * N);
    }
    else
    {
        N = v_Normal;
    }
    vec3 V = normalize(viewPos - v_FragPos);

    vec3 albedo = pow(texture(diffuseTexture, v_TextureCoords).rgb, vec3(2.2)); // sRGB->RGB
    float metallic = texture(metallicTexture, v_TextureCoords).r;
    float roughness = texture(roughnessTexture, v_TextureCoords).r;
    float ao = texture(aoTexture, v_TextureCoords).r;
    vec3 emissive = pow(texture(emissiveTexture, v_TextureCoords).rgb, vec3(2.2)); // sRGB->RGB

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    bool fragmentInShadow = hasShadowMap && calculateShadow(v_FragPosLightSpace);

    if(!fragmentInShadow)
    {
        if(hasDirectionalLight)
            Lo += CalcDirLight(directionalLight, N, V, v_FragPos, albedo, metallic, roughness, emissive, F0);
    }
    for(int i = 0; i < amountPointLights; i++)
        Lo += CalcPointLight(pointLights[i], N, V, v_FragPos, albedo, metallic, roughness, emissive, F0);
    vec3 ambient = vec3(0.005) * albedo;
    
    vec3 color = (Lo + ambient) * pow(ao,2);

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}

bool calculateShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    if(projCoords.z > 1.0)
        return false;

    float bias = 0.005;

    return currentDepth - bias > closestDepth;
}

#endif