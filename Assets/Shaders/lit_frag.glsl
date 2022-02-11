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

struct Material {
    vec3 objectColor;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
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

struct DirectionalLight {
    vec3 lightDir;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 lightPos;
    vec3 lightDir;

    float constant;
    float linear;
    float quadratic;

    float cutoff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};


uniform Model model;
uniform Camera camera;
uniform Material material;

#define POINT_LIGHT_COUNT 5
uniform PointLight[POINT_LIGHT_COUNT] pointLight;
#define DIRECTIONAL_LIGHT_COUNT 3
uniform DirectionalLight[DIRECTIONAL_LIGHT_COUNT] directionalLight;
#define SPOT_LIGHT_COUNT 5
uniform SpotLight[SPOT_LIGHT_COUNT] spotLight;

vec3 calculatePointLights(PointLight light, vec3 ambientCol, vec3 difCol, vec3 specCol, vec3 normal, vec3 viewDir, vec3 fragPos) {
    vec3 lightDir = normalize(light.lightPos - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * ambientCol;
    vec3 diffuse  = light.diffuse  * diff * difCol;
    vec3 specular = light.specular * spec * specCol;
    return clamp((ambient + diffuse + specular) * attenuation, 0.0, 1.0);

}

vec3 calculateDirectionalLights(DirectionalLight light, vec3 ambientCol, vec3 difCol, vec3 specCol, vec3 normal, vec3 viewDir) {
    // ambient
    vec3 ambient = light.ambient * ambientCol;
  	
    // diffuse 
    vec3 lightDir = normalize(-light.lightDir);  
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * difCol;  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specCol;  

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

vec3 calculateSpotLights(SpotLight light,  vec3 ambientCol, vec3 difCol, vec3 specCol, vec3 normal, vec3 viewDir, vec3 fragPos) {
    vec3 lightDir = normalize(light.lightPos - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * ambientCol;
    vec3 diffuse  = light.diffuse  * diff * difCol;
    vec3 specular = light.specular * spec * specCol;

    float theta     = dot(lightDir, normalize(-light.lightDir));
    float epsilon   = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    diffuse *= intensity;
    specular *= intensity;

    return clamp((ambient+ diffuse + specular) * attenuation, 0.0, 1.0);
}

void main()
{
    vec3 normal = normalize(Normal);

    vec3 viewDir = normalize(camera.viewPos - FragPos);
    
    vec3 result = vec3(0.0);


    for(int i = 0; i < POINT_LIGHT_COUNT; i++) {
        result += calculatePointLights(pointLight[i], material.ambient, material.diffuse, material.specular, normal, viewDir, FragPos);
    }
    for(int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
        result += calculateDirectionalLights(directionalLight[i], material.ambient, material.diffuse, material.specular, normal, viewDir);
    }
    for(int i = 0; i < SPOT_LIGHT_COUNT; i++) {
        result += calculateSpotLights(spotLight[i], material.ambient, material.diffuse, material.specular, normal, viewDir, FragPos);
    }

    FragColor = vec4(result, 1.0);
}