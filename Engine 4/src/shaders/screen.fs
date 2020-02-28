#version 430 core

in vec2 UV;

out vec4 color;

uniform sampler2D colTex;

void main()
{
	color = vec4(texture(colTex, UV).rgb, 1);
}
