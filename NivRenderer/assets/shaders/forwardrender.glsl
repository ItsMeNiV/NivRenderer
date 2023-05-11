#version 330 core

#ifdef VERTEX

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertTextureCoords;
layout (location = 3) in vec3 vertTangent;
layout (location = 4) in vec3 vertBitangent;

out vec2 v_TextureCoords;

uniform mat4 model;
uniform mat4 viewProjection;

void main()
{
    v_TextureCoords = vertTextureCoords;
    gl_Position = viewProjection * model * vec4(vertPosition.x, vertPosition.y, vertPosition.z, 1.0);
}

#endif

//=========================//

#ifdef FRAGMENT

in vec2 v_TextureCoords;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

void main()
{
    FragColor = texture(diffuseTexture, v_TextureCoords);
} 

#endif