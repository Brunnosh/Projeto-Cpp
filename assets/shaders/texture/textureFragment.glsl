#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;


uniform sampler2D atlas;
uniform vec3 lightDir;    // Direçao do sol 
uniform vec3 viewPos;     // Posição da câmera
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess = 4;

void main()
{
	vec4 texColor = texture(atlas, TexCoord);

	// Iluminação ambiente
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0); // Luz suave


    // Iluminação difusa
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);  // Cor da luz difusa

    // Iluminação especular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);  // Reflexo da luz


    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, texColor.a);  // Combina a iluminação com a textura  // Combina a iluminação com a textura

  
}
