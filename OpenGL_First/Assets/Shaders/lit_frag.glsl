#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

struct Model {
    mat4 MVP;
    mat4 M;
    mat4 V;
    mat4 P;
    mat4 VP;
    mat4 MV;
    mat4 TIM;
};

struct Camera {

    vec3 viewPos;
};

struct PointLight {
    vec3 lightPos;

    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 objectColor;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Model model;
uniform Camera camera;
uniform PointLight pointLight;
uniform Material material;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  pointLight.diffuse * (diff * material.diffuse);

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * pointLight.ambient;

    float specularStrength = 0.5;

    vec3 viewDir = normalize(camera.viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = pointLight.specular * (spec * material.specular);  

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}