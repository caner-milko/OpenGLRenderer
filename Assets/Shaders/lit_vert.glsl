#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

struct Model {
    mat4 MVP;
    mat4 M;
    mat4 V;
    mat4 P;
    mat4 VP;
    mat4 MV;
    mat4 TIM;
};

uniform Model model;

void main()
{
    gl_Position = model.MVP * vec4(aPos, 1.0);
    FragPos = vec3(model.M * vec4(aPos, 1.0));
    Normal =mat3( model.TIM ) * aNormal;
}