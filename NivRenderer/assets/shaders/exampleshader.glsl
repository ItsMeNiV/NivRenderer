#version 460 core

#ifdef VERTEX

layout (location = 0) in vec3 aPos;


void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#endif

//=========================//

#ifdef FRAGMENT

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
} 

#endif