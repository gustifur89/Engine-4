#version 430 core

in vec3 frg_color;
in vec3 frg_norm;
in vec3 frg_pos;
in float w;
in float z;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 pos;
layout(location = 2) out vec4 norm;

uniform mat4 ColorMatrix;
uniform float specular;

void main()
{
	color = vec4((ColorMatrix * vec4(frg_color, 1.0)).rgb, specular);
	pos = vec4(frg_pos, 0.0);
	norm = vec4(normalize(frg_norm), z/w);	
}