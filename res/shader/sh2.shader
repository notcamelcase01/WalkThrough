#shader vertex
#version 410 core

in vec4 position;
in vec2 texCord;

out vec2 tex;

uniform mat4 model;
void main()
{
	gl_Position =  model*position;
	tex = texCord;
};

#shader fragment
#version 410 core

in vec2 tex;
out vec4 FragColor;

uniform sampler2D texSampler_1;
void main()
{
	FragColor = texture(texSampler_1, tex.xy);
	if (FragColor.r > 0.96 && FragColor.b > 0.96 && FragColor.g > 0.96) {
		FragColor.a = 0.0;
	}
};