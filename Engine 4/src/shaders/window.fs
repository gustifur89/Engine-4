#version 430 core

in vec2 UV;

out vec4 color;

uniform sampler2D colTex;
uniform sampler2D posTex;
uniform sampler2D normTex;
//uniform sampler2D ambientTex;

uniform vec3 globalLight;
uniform mat4 viewMatrix;
uniform mat4 normalViewMatrix;
uniform float ambient;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
	//Get data from gBuffer
	vec4 norm_ = texture( normTex, UV );
	vec4 pos_ = texture( posTex, UV );
	vec4 col_ = texture( colTex, UV );
	vec3 pos = pos_.xyz;
	vec3 norm = norm_.xyz;
	float depth = norm_.w;
	vec3 col = rgb2hsv(col_.xyz);
	float spectral = col_.a;
	bool skyTag = (pos_.w == 1.0) ? true : false;
	//float ambientVal = texture( ambientTex, UV );


	vec3 lightInViewSpace = (viewMatrix * vec4(globalLight,0.0)).xyz;

	vec3 reflectDir = normalize(reflect(-lightInViewSpace, norm)); 
	vec3 viewDir = normalize(-pos);

	float specular = spectral * pow(max(dot(viewDir, reflectDir), 0.0), 0.1 + spectral * 32);

	float diffuse = clamp( dot(lightInViewSpace, norm), 0.0, 1.0);

	float lighting = skyTag ? 1.0 : 0.1 * specular + 0.3 * diffuse + 0.6 * ambient;

	col.b *= lighting;

	color.rgb = hsv2rgb(col);

	color.a = depth;
}