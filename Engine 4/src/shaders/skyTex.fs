#version 330 core

out vec4 FragColor;

in vec3 wolrdSpaceViewDir;

layout(location = 0) out vec4 color;
layout(location = 1) out float depth;

uniform samplerCube skybox;
uniform mat4 ColorMatrix;

void main()
{    
    vec3 dir = normalize(wolrdSpaceViewDir);
    color = vec4((ColorMatrix * vec4(texture(skybox, dir).rgb, 1.0)).rgb, 1.0);
    depth = 1.0;
}