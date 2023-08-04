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

#define MAX_DIR_LIGHTS 32

in vec2 v_TextureCoords;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 FragColor;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform vec3 viewPos;
uniform DirectionalLight directionalLights[MAX_DIR_LIGHTS];

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 normal = normalize(v_Normal);
    vec3 viewDir = normalize(viewPos - v_FragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < 1; i++)
        result += CalcDirLight(directionalLights[i], normal, viewDir);

    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); TODO
    // combine results
    vec3 ambient = 0.2 * light.color * vec3(texture(diffuseTexture, v_TextureCoords));
    vec3 diffuse = light.color * diff * vec3(texture(diffuseTexture, v_TextureCoords));
    //vec3 specular = light.color * spec * vec3(texture(material.specular, TexCoords)); TODO
    return (ambient + diffuse /*+ specular*/);
}

#endif