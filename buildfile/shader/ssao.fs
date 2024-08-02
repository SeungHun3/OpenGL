#version 330 core

out float fragColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform mat4 view;
uniform mat4 projection;

uniform vec2 noiseScale;
uniform float radius;

const int KERNEL_SIZE = 64;
const float BIAS = 0.025;
uniform vec3 samples[KERNEL_SIZE];


void main() 
{
    vec4 worldPos = texture(gPosition, texCoord);
    if (worldPos.w <= 0.0f)
        discard;
    // view space로 전환된 픽셀 worldPos
    vec3 fragPos = (view * vec4(worldPos.xyz, 1.0)).xyz;
    // view space로 전환된 nomal
    vec3 normal = (view * vec4(texture(gNormal, texCoord).xyz, 0.0)).xyz;
    // texNoise로부터 얻어온 randomVector
    vec3 randomVec = texture(texNoise, texCoord * noiseScale).xyz;
    
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;
    for (int i = 0; i < KERNEL_SIZE; i++) 
    {
        vec3 sample = fragPos + TBN * samples[i] * radius;
        vec4 screenSample = projection * vec4(sample, 1.0);
        screenSample.xyz /= screenSample.w;
        screenSample.xyz = screenSample.xyz * 0.5 + 0.5;

        float sampleDepth = (view * texture(gPosition, screenSample.xy)).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + BIAS ? 1.0 : 0.0) * rangeCheck;
    }

    fragColor = 1.0 - occlusion / KERNEL_SIZE;
}
/*
samples 벡터를 랜덤하게 회전된 TBN 행렬 및 현재 픽셀의 3D 위치를 기준으로 변환
해당 sample의 screen 상에서의 xy값을 계산
해당 xy값을 바탕으로 그 위치에 이미 그려진 픽셀의 depth값을 계산
이미 그려진 depth값이 샘플 위치의 depth값보다 크다면 차폐가 발생한 것
설정한 radius보다 멀리 떨어진 샘플이면 영향을 덜 받도록 함
*/