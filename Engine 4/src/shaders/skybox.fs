#version 330 core

out vec4 FragColor;

in vec3 texCoords;

layout(location = 0) out vec4 color;
layout(location = 1) out float depth;

uniform samplerCube skybox;
uniform mat4 ColorMatrix;

void main()
{    
    color = vec4((ColorMatrix * vec4(texture(skybox, texCoords).rgb, 1.0)).rgb, 1.0);
    gl_FragDepth = 1.0;
    depth = 1.0;
}