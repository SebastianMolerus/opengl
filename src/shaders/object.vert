#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 frag_pos_world;
out vec3 frag_pos_view;

void main()
{
    frag_pos_world = vec3(model * vec4(aPos, 1.0));
    frag_pos_view = vec3(view * model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(frag_pos_world, 1.0);
    normal = vec3(model * vec4(aNormal, 0.0));
}; 