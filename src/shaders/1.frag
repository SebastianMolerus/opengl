#version 330 core
out vec4 FragColor;

in vec2 oTexCoords;

uniform sampler2D ourTexture0;
uniform sampler2D ourTexture1;

uniform float mix_value;

void main()
{
   FragColor = mix(texture(ourTexture0, oTexCoords),
   texture(ourTexture1, vec2(-oTexCoords.x, oTexCoords.y)), mix_value);
}