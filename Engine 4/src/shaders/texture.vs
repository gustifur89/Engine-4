#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 uv;

uniform mat4 MVP;
uniform mat4 DepthMVP;
uniform mat4 MV;
uniform mat4 NM;

out vec2 frg_uv;
out vec3 frg_norm;
out vec3 frg_pos;
out float w;
out float z;

void main () 
{
	gl_Position = MVP * vec4(pos, 1);
	frg_uv = uv;
	frg_norm = (NM * vec4(norm, 0)).xyz;
	frg_pos = vec3(MV * vec4(pos,1));
	vec4 depP = DepthMVP * vec4(pos, 1);
	w = depP.w;
	z = depP.z;
}