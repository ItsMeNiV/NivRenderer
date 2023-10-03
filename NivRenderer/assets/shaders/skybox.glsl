#version 460 core

#ifdef VERTEX

layout (location = 0) in vec3 vertPosition;

out vec3 v_TextureCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    v_TextureCoords = vertPosition;
    mat4 myView = mat4(mat3(view));
    vec4 pos = projection * myView * vec4(vertPosition, 1.0);
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
    vec3 color = texture(skybox, v_TextureCoords).rgb;
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}

#endif