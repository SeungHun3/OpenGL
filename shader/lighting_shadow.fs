#version 330 core

out vec4 fragColor;

in VS_OUT 
{
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    vec4 fragPosLight;
} fs_in;

uniform vec3 viewPos;
struct Light 
{
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;
uniform int blinn;
// first pass 에 그린 depth map
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLight) 
{
    //fragPosLight를 바탕으로 light에서 본 픽셀의 위치 계산
    //shadowMap에서부터 해당 픽셀의 depth값 가져오기
    //저장된 depth값이 fragPosLight의 depth값보다 작은 경우 그림자가 생긴 것으로 판단

    // perform perspective divide
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    // [-1, 1] range transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light’s perspective (using
    // [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light’s perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main() 
{
    vec3 texColor = texture2D(material.diffuse, fs_in.texCoord).xyz;
    vec3 ambient = texColor * light.ambient;

    float dist = length(light.position - fs_in.fragPos);
    vec3 distPoly = vec3(1.0, dist, dist*dist);
    float attenuation = 1.0 / dot(distPoly, light.attenuation);
    vec3 lightDir = (light.position - fs_in.fragPos) / dist;

    vec3 result = ambient;
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp(
        (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]),
        0.0, 1.0);

    if (intensity > 0.0) 
    {
        vec3 pixelNorm = normalize(fs_in.normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        vec3 specColor = texture2D(material.specular, fs_in.texCoord).xyz;
        float spec = 0.0;
        if (blinn == 0) 
        {
            vec3 viewDir = normalize(viewPos - fs_in.fragPos);
            vec3 reflectDir = reflect(-lightDir, pixelNorm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        }
        else 
        {
            vec3 viewDir = normalize(viewPos - fs_in.fragPos);
            vec3 halfDir = normalize(lightDir + viewDir);
            spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
        }
        vec3 specular = spec * specColor * light.specular;
        float shadow = ShadowCalculation(fs_in.fragPosLight);

        result += (diffuse + specular) * intensity * (1.0 - shadow);
    }

    result *= attenuation;
    fragColor = vec4(result, 1.0);
}