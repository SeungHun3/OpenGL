#version 330 core

out vec4 fragColor;
in vec3 localPos;

uniform samplerCube cubeMap;
uniform float roughness;

void main() 
{
    // 모든 mip level의 결과 확인용 임시 수정
    vec3 envColor = textureLod(cubeMap, localPos, roughness * 4).rgb;
    envColor = envColor / (envColor + vec3(1.0));   // reinhard
    envColor = pow(envColor, vec3(1.0/2.2));    // to sRGB
    fragColor = vec4(envColor, 1.0);
}