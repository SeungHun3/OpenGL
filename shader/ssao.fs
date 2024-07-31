#version 330 core

out float fragColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;

uniform mat4 view;

void main() 
{
    vec4 worldPos = texture(gPosition, texCoord);
    if (worldPos.w <= 0.0f)
        discard;
    //임시로 view space 상의 y값을 출력
    fragColor = (view * vec4(worldPos.xyz, 1.0)).y * 0.1 + 0.5;
}