#version 330 core
out vec4 FragColor;

in vec2 o_texture_coord;
uniform sampler2D face;
uniform sampler2D container;

void main()
{
    FragColor = mix(texture(container, o_texture_coord), texture(face, o_texture_coord), 0.2);
}