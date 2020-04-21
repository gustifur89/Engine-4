#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 uv;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 NM;
uniform mat4 MM;

out vec2 frg_uv;
out vec3 frg_norm;
out vec3 frg_pos;
out float w;
out float z;

float textureScale = 1.0;

void main () 
{
	gl_Position = MVP * vec4(pos, 1);
	frg_norm = (NM * vec4(norm, 0)).xyz;
	
	frg_pos = vec3(MM * vec4(pos,1)) * textureScale;

	vec3 mPos = mod(pos / textureScale, 1.0);//mod(frg_pos / textureScale, 1.0);

	vec3 absPos = abs(frg_norm);

	vec2 UV;

	////janky for now
	//if(absPos.x > absPos.y && absPos.x > absPos.z)
	//{
	//	UV = frg_pos.yz;
	//}
	//else if(absPos.y > absPos.z)
	//{
	//	UV = frg_pos.zx;
	//}
	//else
	//{
	//	UV = frg_pos.xy;
	//}
	//
	////frg_uv = uv;
	//
	//frg_uv = UV;





	UV = frg_norm.xy/frg_norm.z * 0.5 + 0.5;



	w = gl_Position.w;
	z = gl_Position.z;
}