#version 430 core

in vec2 frg_uv;
in vec3 frg_norm;
in vec3 frg_pos;
in float w;
in float z;

layout(location = 0) out vec4 color;
layout(location = 1) out float depth;

uniform sampler2D tex;
uniform mat4 ColorMatrix;
uniform float specular;

void main()
{
	color = vec4((ColorMatrix * vec4(texture(tex, frg_uv).rgb, 1.0)).rgb, specular);
	depth = z/w;
	float depDif = abs(depth - gl_FragCoord.z);
	//color = vec4(depDif);
	gl_FragDepth = depth;// - gl_FragCoord.z;
}