#version 330 core

layout (location = 0) in vec3 pos;

out vec3 texCoords;

uniform mat4 PVM;

void main()
{
    texCoords = pos;
    vec4 t_pos =  PVM * vec4(pos, 1.0);
    gl_Position = t_pos.xyww;
}