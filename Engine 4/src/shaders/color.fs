#version 430 core

in vec3 frg_color;
in vec3 frg_norm;
in vec3 frg_pos;
in float w;
in float z;

layout(location = 0) out vec4 color;
layout(location = 1) out float depth;

uniform mat4 ColorMatrix;
uniform float specular;

void main()
{
	color = vec4((ColorMatrix * vec4(frg_color, 1.0)).rgb, specular);
	depth = z/w;
	gl_FragDepth = depth;
}