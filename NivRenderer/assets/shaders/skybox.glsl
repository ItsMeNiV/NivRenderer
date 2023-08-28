#version 330 core

#ifdef VERTEX

layout (location = 0) in vec3 vertPosition;

out vec3 v_TextureCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    v_TextureCoords = vertPosition;
    vec4 pos = projection * view * vec4(vertPosition, 1.0);
    gl_Position = pos.xyww;
}

#endif

//=========================//

#ifdef FRAGMENT

out vec4 FragColor;

in vec3 v_TextureCoords;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, v_TextureCoords);
}

#endif