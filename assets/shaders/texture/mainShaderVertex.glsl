#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in float aLightLevel;
//layout (location = 3) in float aSpecularStrenght;
//layout (location = 4) in float aShininess;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out float BlockLightLevel;
//out float MaterialSpecularStrenght;
//out float Shininess;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{

	FragPos = vec3(model * vec4(aPos, 1.0)); // Posição do fragmento no mundo
	Normal = mat3(transpose(inverse(model))) * aNormal;  // Transforma a normal para o world space

	BlockLightLevel = aLightLevel;

	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord = aTexCoord;
}