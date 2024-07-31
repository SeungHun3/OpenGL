#version 330 core

out float fragColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform mat4 view;

uniform vec2 noiseScale;

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
    
    fragColor = tangent.x;
}