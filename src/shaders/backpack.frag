#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D diffuse0;

void main()
{    
    FragColor = texture(diffuse0, TexCoords);
}