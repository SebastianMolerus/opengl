#version 330 core

in vec2 TexCoords;
in vec3 normal_pos_world;
in vec3 frag_pos_world;

struct material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct light_directional
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct light_point
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct light_spot
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    vec3 direction;
    float cut_off;
    float outer_cutoff;
};

uniform material mat;
uniform light_directional directional_light;
uniform light_point point_light;
uniform light_spot spot_light;
uniform vec3 view_pos;

out vec4 FragColor;

vec3 calculate_global_light()
{
    // Ambient
    vec3 ambient = directional_light.ambient * texture(mat.diffuse, TexCoords).rgb;

    // Diffuse
    vec3 norm = normalize(normal_pos_world);
    vec3 light_dir = normalize(-directional_light.direction);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = directional_light.diffuse * diff * texture(mat.diffuse, TexCoords).rgb;

    // Specular
    vec3 view_dir = normalize(view_pos - frag_pos_world);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat.shininess);
    vec3 specular = directional_light.specular * spec * texture(mat.specular, TexCoords).rgb;

    return ambient + diffuse + specular;
}

vec3 calculate_point_light()
{
    // Ambient
    vec3 ambient = point_light.ambient * texture(mat.diffuse, TexCoords).rgb;

    // Diffuse
    vec3 norm = normalize(normal_pos_world);
    vec3 light_dir = normalize(point_light.position - frag_pos_world);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = point_light.diffuse * diff * texture(mat.diffuse, TexCoords).rgb;

    // Specular
    vec3 view_dir = normalize(view_pos - frag_pos_world);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat.shininess);
    vec3 specular = point_light.specular * spec * texture(mat.specular, TexCoords).rgb;

    // Attenuation
    float dist = length(point_light.position - frag_pos_world);
    float attenuation = 1.0 / (point_light.constant + point_light.linear * dist + point_light.quadratic * (dist * dist));

    return vec3(ambient + diffuse + specular) * attenuation;
}

vec3 calculate_spot_light()
{
    vec3 light_dir = normalize(spot_light.position - frag_pos_world);
    float theta = dot(light_dir, normalize(-spot_light.direction));

    float epsilon   = spot_light.cut_off - spot_light.outer_cutoff;
    float intensity = clamp((theta - spot_light.outer_cutoff) / epsilon, 0.0, 1.0); 

    // Ambient
    vec3 ambient = spot_light.ambient * texture(mat.diffuse, TexCoords).rgb;

    // Diffuse
    vec3 norm = normalize(normal_pos_world);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = spot_light.diffuse * diff * texture(mat.diffuse, TexCoords).rgb;

    // Specular
    vec3 view_dir = normalize(view_pos - frag_pos_world);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), mat.shininess);
    vec3 specular = spot_light.specular * spec * texture(mat.specular, TexCoords).rgb;

    return vec3(ambient + intensity * diffuse +  intensity *specular);

    return vec3(0.0);
}

void main()
{
    vec3 result = calculate_global_light();
    result+= calculate_point_light();
    FragColor = vec4(result, 1.0);
}