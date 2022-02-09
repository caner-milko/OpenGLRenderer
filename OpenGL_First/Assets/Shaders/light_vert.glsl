#version 330 core
layout (location = 0) in vec3 aPos;

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
}