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

float textureScale = 1.0;
float sharpness = 1.0;


void main()
{
	//triplanar mapping
	vec3 xCol = texture(tex, frg_pos.yz).rgb;
	vec3 yCol = texture(tex, frg_pos.zx).rgb;
	vec3 zCol = texture(tex, frg_pos.xy).rgb;

	vec3 blend = abs(frg_norm);//pow (abs(frg_norm), sharpness);
	blend = blend / (blend.x + blend.y + blend.z);

	vec3 col = blend.x * xCol + blend.y * yCol + blend.z * zCol;
	color = vec4((ColorMatrix * vec4(col, 1.0)).rgb, specular);
	depth = z/w;
}