#shader vertex
#version 410 core

in vec4 position;




uniform mat4 model;
void main()
{
	gl_Position = model * position;

};

#shader fragment
#version 410 core

out vec4 FragColor;

void main()
{
	FragColor = vec4(0.0,1.0,5.0,1.0);
	
};