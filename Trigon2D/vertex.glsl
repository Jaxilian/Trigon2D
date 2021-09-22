#version 460 core

layout(location = 0) in vec3 inVertexPos;
uniform vec2 inWorldPos;

out vec2 outWorldPos;

void main()
{
    vec2 worldPos = inWorldPos + inVertexPos.xy;

    outWorldPos = inWorldPos;

    gl_Position.xyz = vec3(worldPos, 0);
    gl_Position.w = 1.0;

}