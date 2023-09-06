#version 330 core

#ifdef VERTEX

layout (location = 0) in vec3 vertPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(vertPosition, 1.0);
}

#endif

//=========================//

#ifdef FRAGMENT

void main()
{
    // gl_FragDepth = gl_FragCoord.z; No need for a Fragment shader in Shadowmap
}

#endif