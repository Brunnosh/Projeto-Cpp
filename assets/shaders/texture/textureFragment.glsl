#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
//in float materialSpecularStrenght;

uniform sampler2D atlas;
uniform vec3 lightDir;    // Direçao do sol 
uniform vec3 viewPos;     // Posição da câmera
uniform float ambientStrength;
uniform float shininess = 4;
uniform float sunHeight;

void main()
{
    bool isExposedToSun = true;
    float materialSpecularStrenght = 0.2;

	vec4 texColor = texture(atlas, TexCoord);

    // **Cálculo de iluminação ambiente ajustada pela posição do sol**
    // A posição do sol (sunHeight) pode influenciar o quão intensa é a luz ambiente
    float ambientFactor = mix(0.1f, 1.0f, abs(sunHeight)); // Quando o sol está abaixo do horizonte, o mundo fica mais escuro
    vec3 ambient = ambientFactor * ambientStrength * vec3(1.0, 1.0, 1.0); // Luz ambiente



    // **Cálculo de iluminação difusa**
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);  // Direção da luz (sol)
    float diff = max(dot(norm, lightDirection), 0.0);
    diff = diff * 0.7 + 0.3; // Suavizando a intensidade da sombra
    // Limita a intensidade da luz difusa no meio-dia para evitar clareza excessiva
    diff = min(diff, 1.1); // Limita o valor máximo de difusa para 0.8

    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);  // Cor da luz difusa


    // **Cálculo de iluminação especular**
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    // Interpolando o brilho especular com base na direção do sol (quanto mais alto o sol, mais intenso o brilho)
    float specFactor = mix(0.1f, 0.3f, sunHeight); // Intensidade do brilho especular com base na altura do sol
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = materialSpecularStrenght * specFactor * spec * vec3(1.0, 1.0, 1.0);  // Reflexo da luz

    if (!isExposedToSun) {
        ambient = ambient * 0.2f;  // Se não está exposto ao sol, diminuímos a iluminação
        diffuse = diffuse * 0.5f;  // A difusa também é reduzida
        specular = specular * 0.3f;  // O brilho especular também é atenuado
    }


    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, texColor.a);  // Combina a iluminação com a textura  // Combina a iluminação com a textura

  
}
