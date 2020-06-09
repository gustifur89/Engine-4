#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 MVP;
uniform mat4 MV;

out vec3 wolrdSpaceViewDir;

void main()
{
    gl_Position = MVP * vec4(pos, 1.0);//t_pos.xyzw;  
    wolrdSpaceViewDir = (MV * vec4(pos, 1)).xyz;
}