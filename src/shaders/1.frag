#version 330 core
out vec4 FragColor;

in vec2 o_texture_coord;
uniform sampler2D tile_texture;

void main()
{
   FragColor = texture(tile_texture, o_texture_coord);
}