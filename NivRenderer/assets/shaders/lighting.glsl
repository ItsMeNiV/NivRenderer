struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
    int strength;
};

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float calcLinearTerm(float distance)
{
    return map(distance, 1, 3250, 0.5, 0.0014);
}

float calcQuadraticTerm(float distance)
{
    return map(distance, 1, 3250, 0.5, 0.000007);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, sampler2D diffuseTexture, vec2 textureCoords)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal); TODO
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); TODO
    // combine results
    vec3 ambient = 0.2 * light.color * vec3(texture(diffuseTexture, textureCoords));
    vec3 diffuse = light.color * diff * vec3(texture(diffuseTexture, textureCoords));
    //vec3 specular = light.color * spec * vec3(texture(material.specular, TexCoords)); TODO
    return ambient + diffuse; //+ specular
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, sampler2D diffuseTexture, vec2 textureCoords)
{
    // ambient
    vec3 ambient = 0.2 * light.color * texture(diffuseTexture, textureCoords).rgb;
  	
    // diffuse 
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffuseTexture, textureCoords).rgb;  
    
    // specular
    //vec3 reflectDir = reflect(-lightDir, norm); TODO
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); TODO
    //vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb; TODO
    
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + calcLinearTerm(distance) * distance + calcQuadraticTerm(distance) * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    //specular *= attenuation;   
        
    return ambient + diffuse;// + specular;
}