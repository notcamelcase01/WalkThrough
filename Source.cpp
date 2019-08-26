#include <GL/glew.h>     // use this before GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Windows.h>
#include "Buffer.h"
#include "shader.h"
#include "Camera.h"
#include "cmath"
using namespace std;


enum Texture { FLOOR = 1, GUN = 2, WALL = 3, FRAMEBUFFER = 4 };
int gravityAct = 0, gCounter = 0;
int kolor = 0;
GLFWwindow* window;
float screenWidth = 500, screenHeight = 500;
float lastX = 400, lastY = 300;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection;
glm::mat4 mvp;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f));

void ProcessInput();
void DrawTestRectangle(Shader& shader, VertexBuffer& vb);
void DrawRectangle(Shader& shader, VertexBuffer& vb, int tex);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void DrawGun(Shader& shader, VertexBuffer& vb, int tex);

void DrawMap(Shader& shader, VertexBuffer& vb, int tex);

glm::vec3 lightPos(0.f, 5.0f, 0.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
float lumins = 0.5f;
void makeCircleVertices(glm::vec3 positions);
float positionsC[300];
unsigned int indicesC[100];

#include<tuple>
tuple< GLuint, GLuint, GLuint> CreateFrameBuffer()
{
	GLuint color_tex = 0, fb = 0, depth_tex = 0;
	//RGBA8 2D texture, 24 bit depth texture, 256x256
	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	//-------------------------
	glGenFramebuffersEXT(1, &fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	//Attach 2D texture to this FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0/*mipmap level*/);
	//-------------------------
	//Attach depth texture to FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_tex, 0/*mipmap level*/);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return tuple< GLuint, GLuint, GLuint>(fb, color_tex, depth_tex);
}

GLuint loadBMP_STB(const char* imagepath)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(imagepath, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}

void DrawCircle(Shader& shader, VertexBuffer& vb, IndexBuffer& ib, VertexBuffer& vbC, IndexBuffer& ibC);



int main(void)
{

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	window = glfwCreateWindow(mode->width, mode->height, "Hello World", glfwGetPrimaryMonitor(), NULL);
	screenHeight = (float)mode->height; //Resolution
	screenWidth = (float)mode->width; //Resolution

	//window = glfwCreateWindow(500, 500, "Hello World", NULL, NULL);


	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (GLEW_OK != glewInit())
	{
		cout << "Glew Init Error";
		return -1;
	}

	lastX = screenHeight / 2.;
	lastY = screenWidth / 2.;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	float x = 1.f, y = 1.f, z = -1.f;

	float positions[] = {
		//Cords     //TexCords  //Normal Plane
		-x,  y,  z, 0.0f, 0.0f, 0.0, 0.0, 1.0,//0
		 x,  y,  z, 1.0f, 0.0f, 0.0, 0.0, 1.0,//1
		 x, -y,  z, 1.0f, 1.0f, 0.0, 0.0, 1.0,//2
		-x, -y,  z, 0.0f, 1.0f, 0.0, 0.0, 1.0 //3

	};

	unsigned int indices[] = {
		0, 3, 2,
		0, 1, 2
	};



	VertexBuffer vb(positions, sizeof(positions));

	IndexBuffer ib(indices, 6);



	glEnable(GL_TEXTURE_2D);


	glActiveTexture(GL_TEXTURE0);
	GLuint tid_1 = loadBMP_STB("C:\\a\\s1.jpg");
	glBindTexture(GL_TEXTURE_2D, tid_1);
	glActiveTexture(GL_TEXTURE1);
	GLuint tid_2 = loadBMP_STB("C:\\a\\floor_1.png");
	glBindTexture(GL_TEXTURE_2D, tid_2);
	glActiveTexture(GL_TEXTURE2);
	GLuint tid_3 = loadBMP_STB("C:\\a\\amangun.jpg");
	glBindTexture(GL_TEXTURE_2D, tid_3);
	glActiveTexture(GL_TEXTURE3);
	GLuint tid_4 = loadBMP_STB("C:\\a\\wall_1.bmp");
	glBindTexture(GL_TEXTURE_2D, tid_4);
	glActiveTexture(GL_TEXTURE4);
	auto fb = CreateFrameBuffer();
	glBindTexture(GL_TEXTURE_2D, get<1>(fb));

	Shader shader("res\\shader\\sh.shader");
	Shader shader2("res\\shader\\sh2.shader");
	Shader shader3("res\\shader\\sh0.shader");

	//makeCircleVertices(glm::vec3(0.0,5.0,0.0));
//	VertexBuffer vbC(positionsC, sizeof(positionsC));
	//IndexBuffer ibC(indicesC, 100);
	//Initilization
	{
		model = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
	}
	/* Loop until the user closes the window */
	float gBounce[] = { 0.5,0.5,-0.5,-0.2,-0.2,-0.1,0.0 };
	while (!glfwWindowShouldClose(window))
	{

		if (kolor == 0) {
			lightColor = glm::vec3(1.0, 1.0, 1.0); //Wl
		}
		else if (kolor == 1) {
			lightColor = glm::vec3(1.0, 0.5764, 0.1608); //Tungsten
		}
		else if (kolor == 2) {
			lightColor = glm::vec3(0.251, 0.612, 0.5607); //Can
		}
		else {
			lightColor = glm::vec3(1.0, 0.84, 0.667);  //B
		}
		/* Render here */
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		ProcessInput();

		glBindFramebufferEXT(GL_FRAMEBUFFER, get<0>(fb));


		glEnable(GL_DEPTH_BUFFER_BIT | GL_BLEND);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		//Matrix Update
		projection = glm::perspective(glm::radians(camera.Zoom), screenWidth / screenHeight, 0.1f, 100.0f);
		view = glm::lookAt(glm::vec3(0.0, 25.0, 0.0), glm::vec3(0.0, 25.0, 0.0) + glm::vec3(0.0, -1.0, 0.0), glm::vec3(1.0, 0.0, 0.0));
		//End Matrix Update

		//IB1 ----------------------------------------------------------------------------------------- IB1//
		vb.Bind();
		ib.Bind();
		for (int i = -5; i < 5; i++) {
			for (int j = -5; j < 5; j++) {
				model = glm::mat4(1.0);
				model = glm::translate(model, glm::vec3((float)(2 * i), -3.0, (float)(2 * j)));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
				DrawRectangle(shader, vb, Texture::FLOOR);
			}
		}

		for (int i = -5; i < 5; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3((float)(-1 * (2 * 5)), -1.0, (float)(2 * i)));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			DrawRectangle(shader, vb, Texture::WALL);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3((float)(1 * (2 * 5)), -1.0, (float)(2 * i)));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			DrawRectangle(shader, vb, Texture::WALL);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3((float)(2 * i), -1.0, (float)(1 * (2 * 5))));
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
			DrawRectangle(shader, vb, Texture::WALL);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3((float)(2 * i), -1.0, (float)(-1 * (2 * 5))));
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
			DrawRectangle(shader, vb, Texture::WALL);
		}



		DrawTestRectangle(shader, vb);

		//IB2 ----------------------------------------------------------------------------------------- IB2//


		glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

		vb.Bind();
		ib.Bind();

		glEnable(GL_DEPTH_BUFFER_BIT | GL_BLEND);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		//Matrix Update
		projection = glm::perspective(glm::radians(camera.Zoom), screenWidth / screenHeight, 0.1f, 100.0f);

		if (gravityAct != 0)
		{
			if (gBounce[gCounter] == 0.0)
			{
				gravityAct = 0;
				gCounter = 0;
				camera.free = false;
			}
			else {
				camera.Position.y = camera.Position.y + gBounce[gCounter];
				gCounter++;
			}

		}

		view = camera.GetViewMatrix();
		//End Matrix Update

		//IB1 ----------------------------------------------------------------------------------------- IB1//
		vb.Bind();
		ib.Bind();
		for (int i = -5; i < 5; i++) {
			for (int j = -5; j < 5; j++) {
				model = glm::mat4(1.0);
				model = glm::translate(model, glm::vec3((float)(2 * i), -3.0, (float)(2 * j)));
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
				DrawRectangle(shader, vb, Texture::FLOOR);
			}
		}

		for (int i = -5; i < 5; i++) {
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3((float)(-1 * (2 * 5)), -1.0, (float)(2 * i)));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			DrawRectangle(shader, vb, Texture::WALL);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3((float)(1 * (2 * 5)), -1.0, (float)(2 * i)));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			DrawRectangle(shader, vb, Texture::WALL);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3((float)(2 * i), -1.0, (float)(1 * (2 * 5))));
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
			DrawRectangle(shader, vb, Texture::WALL);
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3((float)(2 * i), -1.0, (float)(-1 * (2 * 5))));
			model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
			DrawRectangle(shader, vb, Texture::WALL);
		}



		DrawGun(shader2, vb, Texture::GUN);

		//IB2 ----------------------------------------------------------------------------------------- IB2//
		DrawMap(shader2, vb, Texture::FRAMEBUFFER);
	//	DrawCircle(shader3, vb, ib, vbC, ibC);


		/* Swap front and back buffers */
		glfwSwapBuffers(window);
	
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void makeCircleVertices(glm::vec3 position) {

	float radius = 1.f;
	float sectors = 10.f;
	float stacks = 10.f;
	float pitchAngle = 360. / sectors;
	float yawAngle = 180. / stacks;
	float sum = 0.;
	for (float i = 0.; i < 90.; i += yawAngle) {
		sum += cos(glm::radians(i));
	}

	float dr = radius / sum;
	int cI = 0;
	int currentIndice = 0;

	    positionsC[0] = position.x; currentIndice++;
		positionsC[1] = position.y + radius; currentIndice++;
		positionsC[2] = position.z;  currentIndice++;
		indicesC[cI] = cI++;
	for (float i = 0.; i <= 360.; i+= pitchAngle) {
		for (float j = 0.; j <= 180.; j += yawAngle) {
			positionsC[currentIndice + 1] = positionsC[currentIndice - 3] + dr * sin(glm::radians(i)); currentIndice++;
			positionsC[currentIndice + 1] = positionsC[currentIndice - 3] + dr * sin(glm::radians(j)); currentIndice++;
			positionsC[currentIndice + 1] = positionsC[currentIndice - 3] + dr * cos(glm::radians(j)); currentIndice++;
			indicesC[cI] = cI++;
		}
	}
}


	


void DrawCircle(Shader& shader,VertexBuffer& vb, IndexBuffer& ib,VertexBuffer& vbC,IndexBuffer& ibC) {
	vbC.Bind();
	ibC.Bind();
	VertixAttrib va;
	BufferLayout layout(3);
	shader.Bind();
	GLint posAttrib = shader.getAttribLocation("position");
	layout.Push<float>(posAttrib, 3, false);
	va.AddBuffer(vbC, layout);
	glm::mat4 modelx = glm::mat4(1.0f);
	modelx = projection * view * model;
	shader.setUniform4fv("model", modelx);
	glDrawElements(GL_TRIANGLES, 100, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
	vb.Bind();
	ib.Bind();
}

void DrawMap(Shader& shader, VertexBuffer& vb, int tex) {
	VertixAttrib va;
	BufferLayout layout(8);



	shader.Bind();



	GLint posAttrib = shader.getAttribLocation("position");
	layout.Push<float>(posAttrib, 3, false);


	posAttrib = shader.getAttribLocation("texCord");
	layout.Push<float>(posAttrib, 2, false);



	va.AddBuffer(vb, layout);
	glm::mat4 modelx = glm::mat4(1.0f);
	modelx = glm::scale(modelx, glm::vec3(.25, .25, 1.));
	modelx = glm::translate(modelx, glm::vec3(-3.3, -3., 0.));

	shader.setUniform4fv("model", modelx);
	shader.setUniform1i("texSampler_1", tex);


	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
}

void DrawTestRectangle(Shader& shader, VertexBuffer& vb)
{
	VertixAttrib va;
	BufferLayout layout(8);


	shader.Bind();


	GLint posAttrib = shader.getAttribLocation("position");
	layout.Push<float>(posAttrib, 3, false);


	posAttrib = shader.getAttribLocation("texCord");
	layout.Push<float>(posAttrib, 2, false);


	posAttrib = shader.getAttribLocation("aNormal");
	layout.Push<float>(posAttrib, 3, false);

	va.AddBuffer(vb, layout);
	model = glm::mat4(1.0);
	model = glm::translate(model, camera.Position);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));

	model = glm::scale(model, glm::vec3(1.5, 1.5, 0.0));

	mvp = projection * view * model;
	shader.setUniform4fv("mvp", mvp);
	shader.setUniform4fv("model", model);
	shader.setUniform1i("texSampler_1", 0);
	shader.setUniform3fv("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setUniform3fv("lightPos", lightPos);
	shader.setUniform3fv("viewPos", camera.Position);
	shader.setUniform1f("ambientStrength", 1.0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

}

void DrawRectangle(Shader& shader, VertexBuffer& vb, int tex)
{
	VertixAttrib va;
	BufferLayout layout(8);



	shader.Bind();



	GLint posAttrib = shader.getAttribLocation("position");
	layout.Push<float>(posAttrib, 3, false);


	posAttrib = shader.getAttribLocation("texCord");
	layout.Push<float>(posAttrib, 2, false);


	posAttrib = shader.getAttribLocation("aNormal");
	layout.Push<float>(posAttrib, 3, false);

	va.AddBuffer(vb, layout);
	mvp = projection * view * model;
	shader.setUniform4fv("mvp", mvp);
	shader.setUniform4fv("model", model);
	shader.setUniform1i("texSampler_1", tex);
	shader.setUniform3fv("lightColor", lightColor);
	shader.setUniform3fv("lightPos", lightPos);
	shader.setUniform3fv("viewPos", camera.Position);
	shader.setUniform1f("ambientStrength", lumins);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

}

void DrawGun(Shader& shader, VertexBuffer& vb, int tex)
{
	VertixAttrib va;
	BufferLayout layout(8);



	shader.Bind();



	GLint posAttrib = shader.getAttribLocation("position");
	layout.Push<float>(posAttrib, 3, false);


	posAttrib = shader.getAttribLocation("texCord");
	layout.Push<float>(posAttrib, 2, false);



	va.AddBuffer(vb, layout);
	glm::mat4 modelx = glm::mat4(1.0f);
	modelx = glm::scale(modelx, glm::vec3(.5, .5, .5));
	modelx = glm::translate(modelx, glm::vec3(1., -1., 0));

	shader.setUniform4fv("model", modelx);
	shader.setUniform1i("texSampler_1", tex);


	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

}



void ProcessInput()
{
	float cameraSpeed = 4.f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::SWITCH_FREEWILL, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		lumins += 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		lumins -= 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		kolor = (kolor + 1) % 4;
		Sleep(1000);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		gravityAct = 1;
		camera.free = true;
		Sleep(50);
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		exit(0);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

//  whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	camera.ProcessMouseScroll(yoffset);

}