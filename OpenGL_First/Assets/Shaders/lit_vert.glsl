#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 TIM;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
    FragPos = vec3(M * vec4(aPos, 1.0));
    Normal =mat3( TIM ) * aNormal;
}