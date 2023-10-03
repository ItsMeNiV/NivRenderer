#version 460 core

#ifdef VERTEX

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertTextureCoords;
layout (location = 3) in vec3 vertTangent;
layout (location = 4) in vec3 vertBitangent;

#include "shareduniforms.glsl"

void main()
{
    gl_Position = viewProjection * model * vec4(vertPosition, 1.0);
}

#endif

//=========================//

#ifdef FRAGMENT

out vec4 FragColor;

uniform vec3 lightColor;

void main()
{
    FragColor = vec4(lightColor, 1.0);
}

#endif