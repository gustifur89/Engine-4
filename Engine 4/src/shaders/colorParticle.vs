#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 pos;
layout (location = 2) in vec4 col;

uniform mat4 MVP;
uniform mat4 DepthMVP;
uniform mat4 MV;
uniform mat4 NM;

out vec3 frg_color;
out vec3 frg_norm;
out vec3 frg_pos;
out float w;
out float z;

void main () 
{
	float size = pos.w;
	vec4 sPos = MVP * vec4(pos.xyz, 1);
	gl_Position = sPos + vec4(size * vPos.xy, 0, 0);

	//vec3 mPos = size * vPos + pos.xyz;
	//gl_Position = MVP * vec4(mPos, 1);
	frg_color = col.rgb;
	z = gl_Position.z;
	w = gl_Position.w;
}