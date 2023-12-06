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

#customcode

#endif