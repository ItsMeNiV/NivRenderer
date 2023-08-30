struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
    int strength;
};

#ifdef USE_PBR
// Using Cook-Torrance BRDF

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a2 = roughness * roughness * roughness * roughness; // According to Disney
    float NdotH = max(dot(N, H), 0.0);

    float denom = (NdotH * NdotH) * (a2 - 1) + 1;

    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float dotProduct, float roughness)
{
    float a2 = (roughness + 1) * (roughness + 1);
    float K = a2 / 8;
    return dotProduct / (dotProduct * (1 - K) + K);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float part1 = GeometrySchlickGGX(NdotV, roughness);
    float part2 = GeometrySchlickGGX(NdotL, roughness);

    return part1 * part2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5);
}

vec3 CalcDirLight(DirectionalLight light, vec3 N, vec3 V, vec3 fragPos, vec3 albedo, float metallic, float roughness, vec3 emissive, vec3 F0)
{
    vec3 L = normalize(-light.direction);
    vec3 lightIntensity = light.color;

    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float VdotH = max(dot(V, H), 0.0);

    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);
    vec3 F = FresnelSchlick(VdotH, F0);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    vec3 specularBRDF = (D * G * F) / 4 * NdotL * NdotV + 0.0001;

    vec3 fLambert = vec3(0.0);
    if(metallic < 0.1)
        fLambert = albedo;

    vec3 diffuseBRDF = kD * fLambert / PI;

    return emissive + ((diffuseBRDF + specularBRDF) * lightIntensity * NdotL);
}

vec3 CalcPointLight(PointLight light, vec3 N, vec3 V, vec3 fragPos, vec3 albedo, float metallic, float roughness, vec3 emissive, vec3 F0)
{
    vec3 L = light.position - fragPos;
    float lightDist = length(L);
    L = normalize(L);
    vec3 lightIntensity = light.color * (light.strength / 5.0f);
    lightIntensity /= (lightDist * lightDist);

    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    float VdotH = max(dot(V, H), 0.0);

    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);
    vec3 F = FresnelSchlick(VdotH, F0);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    vec3 specularBRDF = (D * G * F) / 4 * NdotL * NdotV + 0.0001;

    vec3 fLambert = albedo;

    vec3 diffuseBRDF = kD * fLambert / PI;

    return emissive + ((diffuseBRDF + specularBRDF) * lightIntensity * NdotL);
}

#else

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

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, sampler2D diffuseTexture, bool hasSpecularTexture, sampler2D specularTexture, vec2 textureCoords)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    // combine results
    vec3 ambient = 0.2 * light.color * texture(diffuseTexture, textureCoords).rgb;
    vec3 diffuse = light.color * diff * texture(diffuseTexture, textureCoords).rgb;
    float specularValue = hasSpecularTexture ? texture(specularTexture, textureCoords).r : 0.0;
    vec3 specular = light.color * spec * specularValue;
    return ambient + diffuse + specular;
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, sampler2D diffuseTexture, bool hasSpecularTexture, sampler2D specularTexture, vec2 textureCoords)
{
    // ambient
    vec3 ambient = 0.2 * light.color * texture(diffuseTexture, textureCoords).rgb;
  	
    // diffuse 
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(diffuseTexture, textureCoords).rgb;  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    float specularValue = hasSpecularTexture ? texture(specularTexture, textureCoords).r : 0.0;
    vec3 specular = light.color * spec * specularValue;
    
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = ((light.strength/100.0) * 10.0) * (0.4 / (1.0 + calcLinearTerm(distance) * distance + calcQuadraticTerm(distance) * (distance * distance)));

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    return ambient + diffuse + specular;
}
#endif