#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
//in float materialSpecularStrenght;

uniform sampler2D atlas;
uniform vec3 lightDir;    // Dire�ao do sol 
uniform vec3 viewPos;     // Posi��o da c�mera
uniform float ambientStrength;
uniform float shininess = 4;
uniform float sunHeight;

void main()
{
    bool isExposedToSun = true;
    float materialSpecularStrenght = 0.2;

	vec4 texColor = texture(atlas, TexCoord);

    // **C�lculo de ilumina��o ambiente ajustada pela posi��o do sol**
    // A posi��o do sol (sunHeight) pode influenciar o qu�o intensa � a luz ambiente
    float ambientFactor = mix(0.1f, 1.0f, abs(sunHeight)); // Quando o sol est� abaixo do horizonte, o mundo fica mais escuro
    vec3 ambient = ambientFactor * ambientStrength * vec3(1.0, 1.0, 1.0); // Luz ambiente



    // **C�lculo de ilumina��o difusa**
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);  // Dire��o da luz (sol)
    float diff = max(dot(norm, lightDirection), 0.0);
    diff = diff * 0.7 + 0.3; // Suavizando a intensidade da sombra
    // Limita a intensidade da luz difusa no meio-dia para evitar clareza excessiva
    diff = min(diff, 1.1); // Limita o valor m�ximo de difusa para 0.8

    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);  // Cor da luz difusa


    // **C�lculo de ilumina��o especular**
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    // Interpolando o brilho especular com base na dire��o do sol (quanto mais alto o sol, mais intenso o brilho)
    float specFactor = mix(0.1f, 0.3f, sunHeight); // Intensidade do brilho especular com base na altura do sol
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = materialSpecularStrenght * specFactor * spec * vec3(1.0, 1.0, 1.0);  // Reflexo da luz

    if (!isExposedToSun) {
        ambient = ambient * 0.2f;  // Se n�o est� exposto ao sol, diminu�mos a ilumina��o
        diffuse = diffuse * 0.5f;  // A difusa tamb�m � reduzida
        specular = specular * 0.3f;  // O brilho especular tamb�m � atenuado
    }


    vec3 result = (ambient + diffuse + specular) * texColor.rgb;
    FragColor = vec4(result, texColor.a);  // Combina a ilumina��o com a textura  // Combina a ilumina��o com a textura

  
}
