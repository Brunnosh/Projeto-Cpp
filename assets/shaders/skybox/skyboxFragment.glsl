#version 330 core
in vec3 TexCoord;

out vec4 FragColor;

uniform samplerCube skybox;  // Textura da skybox
uniform vec3 dayColor;
uniform vec3 nightColor;

void main()
{
    vec3 color = mix(nightColor, dayColor, TexCoord.y);  // Dependendo da posição Y da skybox, muda a cor
    FragColor = vec4(color, 1.0f);
}
