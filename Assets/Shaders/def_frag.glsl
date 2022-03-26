#version 330 core
out vec4 FragColor;

in vec2 texCoords;

struct Material{
    sampler2D texture1;
    sampler2D texture2;
};
uniform Material material;
uniform vec2 texCoordsOffset;

void main()
{
    vec4 tex2 = texture(material.texture2, texCoords);
    FragColor = mix(texture(material.texture1, texCoords), tex2, tex2.a);
} 