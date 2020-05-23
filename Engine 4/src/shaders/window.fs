#version 430 core

in vec2 UV;

out vec4 color;

uniform sampler2D colTex;
uniform sampler2D depthTex;

void main()
{
	vec4 col_ = texture( colTex, UV );
	float depth = texture( depthTex, UV ).r;
	if(UV.x < 0.5)
	{
		color.rgb = vec3(depth);// col_.rgb;
	}
	else
	{
		color.rgb = col_.rgb;
	}
	color.rgb = col_.rgb;
	color.a = 1.0;
}