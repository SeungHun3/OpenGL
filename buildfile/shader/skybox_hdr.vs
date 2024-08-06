#version 330 core

layout (location = 0) in vec3 aPos;
out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main() 
{
    localPos = aPos;
    mat4 rotView = mat4(mat3(view)); // remove translation
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);
    gl_Position = clipPos.xyww; //fragment shader에서 z값이 항상 1이 되도록 -> 추후 depth버퍼 계산시 가장 멀리 있는 물체로 인식
}