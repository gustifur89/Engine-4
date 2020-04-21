#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 MVP;
uniform mat4 MV;

out vec2 UV;
out float w;

void main(){
	vec4 tempPos = MVP * vec4(pos, 1);
	gl_Position = tempPos;
	UV = tempPos.xy;
	w = tempPos.w;
}

