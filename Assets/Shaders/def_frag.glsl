#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec2 texCoordsOffset;

void main()
{
    vec4 tex2 = texture(texture2, texCoords);
    FragColor = mix(texture(texture1, texCoords), tex2, tex2.a);
} 