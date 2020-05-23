#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 MVP;
uniform mat4 DepthMVP;
uniform mat4 MV;

out vec2 UV;
out float w;
out float z;

void main(){
	vec4 tempPos = MVP * vec4(pos, 1);
	gl_Position = tempPos;
	UV = tempPos.xy;
	vec4 depP = DepthMVP * vec4(pos, 1);
	w = depP.w;
	z = depP.z;
}

