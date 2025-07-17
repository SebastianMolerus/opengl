#version 330 core

struct material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct light_src
{
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform light_src light;
uniform material mat;

out vec4 FragColor;

in vec3 normal;
in vec3 frag_pos_world;
in vec3 frag_pos_view;

void main()
{
    // Ambient
    vec3 ambient = light.ambient * mat.ambient;

    float len = length(light.pos - frag_pos_world);

    // Diff
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light.pos - frag_pos_world);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light.diffuse * mat.diffuse / len;
   
    // Specular
    vec3 view_dir = normalize(-frag_pos_view);
    vec3 reflect_dir = reflect(-light_dir, norm);
    // ^32
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat.shininess);
    vec3 specular = diff * light.specular * spec * mat.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}