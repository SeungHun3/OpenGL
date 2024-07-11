#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos;

struct Light {
    vec3 position;
    vec3 direction;
    float cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
 
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

void main() {
    vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    vec3 ambient = texColor * light.ambient;
 
    float dist = length(light.position - position);
    vec3 distPoly = vec3(1.0, dist, dist*dist); // 1 , d, d*d
    // Attenuation Model 계산식
    // 1 / ( Kc + Kl*d + Kq*(d*d) ) =>
    // 1 /
    // ( distPoly.x * x light.attenuation.x + 
    //   distPoly.y * x light.attenuation.y + 
    //   distPoly.z * x light.attenuation.z )

    float attenuation = 1.0 / dot(distPoly, light.attenuation);
    vec3 lightDir = (light.position - position) / dist;

    // 라이트가 바라보는 방향과 물체와 라이트의 방향의 값을 theta로 넣음
    float theta = dot(lightDir, normalize(-light.direction));
    vec3 result = ambient;

    if (theta > light.cutoff) 
    {
        // light가 설정한 각도(cos값)과 비교하여 값이 각도안에 존재할 경우에만 빛 적용
        // theta의 값이 더 크다면 == 설정한 각도cutoff의 범위보다 작다면
        vec3 pixelNorm = normalize(normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        vec3 specColor = texture2D(material.specular, texCoord).xyz;
        vec3 viewDir = normalize(viewPos - position);
        vec3 reflectDir = reflect(-lightDir, pixelNorm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = spec * specColor * light.specular;

        result += diffuse + specular;
    }
    // 감쇠적용
    result *= attenuation;
    fragColor = vec4(result, 1.0);
}