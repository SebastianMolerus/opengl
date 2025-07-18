#version 330 core

in vec2 TexCoords;
in vec3 normal_pos_world;
in vec3 frag_pos_world;

struct light_src
{
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform light_src light;

struct material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform material mat;

uniform vec3 view_pos;

out vec4 FragColor;

void main()
{
    // Ambient
    vec3 ambient = light.ambient * vec3(texture(mat.diffuse, TexCoords));
    // Diff
    vec3 norm = normalize(normal_pos_world);
    vec3 light_dir = normalize(light.pos - frag_pos_world);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, TexCoords));

    vec3 view_dir = normalize(view_pos - frag_pos_world);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat.shininess);
    vec3 specular = light.specular * spec * texture(mat.specular, TexCoords).rgb;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}