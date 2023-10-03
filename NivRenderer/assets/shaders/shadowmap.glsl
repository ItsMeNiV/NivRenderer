#version 460 core

#ifdef VERTEX

layout (location = 0) in vec3 vertPosition;

#include "shareduniforms.glsl"

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