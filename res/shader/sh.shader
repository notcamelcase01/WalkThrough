#shader vertex
#version 410 core

in vec4 position;
in vec2 texCord;
in vec3 aNormal;
out vec2 tex;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 mvp;
void main()
{
	FragPos = vec3(model * vec4(position));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	gl_Position = mvp * position;
	tex = texCord;
};

#shader fragment
#version 410 core

in vec2 tex;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

out vec4 FragColor;

uniform float ambientStrength;
uniform sampler2D texSampler_1;
void main()
{
	FragColor = texture(texSampler_1, tex.xy);
	
	vec3 ambient = ambientStrength * lightColor;

	// diffuse 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * (FragColor.r,FragColor.g,FragColor.b);
	FragColor = vec4(result, 1.0);
};