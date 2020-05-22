#version 450 core

in vec2 UV;
in float w;
in float z;

layout(location = 0) out vec4 color;
layout(location = 1) out float depth;

uniform mat4 ColorMatrix;
uniform sampler2D colTex;
uniform sampler2D depthTex;

void main()
{
	vec2 uv = ((UV/w)+vec2(1,1)) /2.0;
	vec4 col = texture(colTex, uv);
	//color = ColorMatrix * col;
	color = col;
	//depth = texture(depthTex, uv).r;
	depth = z/w;
}