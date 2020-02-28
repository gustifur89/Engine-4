#version 330 core

in vec2 UV;

out float ao;

uniform sampler2D posTex;
uniform sampler2D normTex;
uniform sampler2D texNoise;

#define KERNAL_SIZE 32

uniform vec3 samples[KERNAL_SIZE];
uniform mat4 projection;
uniform mat4 viewMatrix;

uniform vec2 noiseScale;
uniform vec2 windowScale;

const int kernelSize = 64;
const float radius = 0.5;
const float bias = 0.08;
float gSampleRad = 0.6;


uniform float zNear = 0.1;
uniform float zFar = 1000.0;


void main()
{
   float occlusion = 0.0;

	vec3 fragPos   = texture(posTex, UV).xyz;
	vec3 normal    = texture(normTex, UV).xyz;
	vec3 randomVec = texture(texNoise, UV * noiseScale).xyz;  

    float rad = radius / fragPos.z;

    for(int i = 0; i < KERNAL_SIZE; i++)
    {
       // vec2 coords = samples[i].xy;

         vec2 coords = reflect(samples[i].xy, randomVec.xy) * rad;

         vec3 diff = texture(posTex, coords + UV).xyz - fragPos;
         vec3 v = normalize(diff);
         float d = length(diff);
         
         occlusion += max(0.0, dot(normal, v) - bias)*(1.0 / (1.0 + d));
	}
    ao = 1.0 - (occlusion / KERNAL_SIZE);
}

