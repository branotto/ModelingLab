/*
* This code is based on the tutorial and source code at 
* learnopengl.com and Lynda.com's "Up and Running with OpenGL" course
* Modified by Brandon Otto to demonstrate modeling.
* The program displays a scene filled with cones and 
* allows the user to navigate the scene using the 
* WASD keys and look around with the mouse.
*/

//standard includes
#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Class Includes
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_Movement();

// Window dimensions
const GLuint WIDTH = 1200, HEIGHT = 900;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 5.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(1.0f, 1.0f, 1.0f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

//macro to write shader programs inline
#ifndef GLSL 
#define GLSL(version,A) "#version " #version "\n" #A
#endif

//Vertex Shader
const GLchar* vertexShaderSource = GLSL(330,
	layout(location = 0) in vec3 position;
	layout(location = 1) in vec3 normal;

	//outputs
	out vec3 FragPos;
	out vec3 Normal;

	//uniforms
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
		
	void main()
	{
		//calcuate the position
		gl_Position = projection * view * model * vec4(position, 1.0f);

		//calculate the fragment position
		FragPos = vec3(model * vec4(position, 1.0f));

		//calculate the normal accounting for tranformations
		Normal = mat3(transpose(inverse(model))) * normal;
	}
);

//Fragment Shader
const GLchar* fragmentShaderSource = GLSL(330,

	//light source
	struct Light 
	{
		vec3 position;

		//coefficients for lighting
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
	};

	//output
	out vec4 color;

	//inputs
	in vec3 Normal;
	in vec3 FragPos;

	//uniforms
	uniform vec3 viewPos;
	uniform Light light;
	uniform vec3 objectColor;


	void main()
	{
		// Ambient
		vec3 ambient = light.ambient;

		// Diffuse 
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(light.position - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * diff;

		// Specular
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = light.specular * spec;

		//calculate the final color
		vec3 result = (ambient + diffuse + specular) * objectColor;
		color = vec4(result, 1.0f);
	}
);

// The MAIN function
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object 
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Modeling Lab", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// 3D
	glEnable(GL_DEPTH_TEST);

	// Build and compile our shader program
	Shader ourShader(vertexShaderSource, fragmentShaderSource);
	
	//Create the normals
	glm::vec3 normal0 = glm::vec3(.577, .577, .577);
	glm::vec3 normal1 = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 normal2 = glm::vec3(-1.0, 0.0, 0.0);
	glm::vec3 normal3 = glm::vec3(0.0, -1.0, 0.0);
	
	//Create the vertices
	glm::vec3 vertex0 = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 vertex1 = glm::vec3(1.0, 0.0, 0.0);
	glm::vec3 vertex2 = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 vertex3 = glm::vec3(0.0, 0.0, 1.0);

	vector<Vertex> vertices;

	Vertex A, B, C, AA, BB, CC, AAA, BBB, CCC, AAAA, BBBB, CCCC; 
	A.Position = vertex1;
	A.Normal = normal0;

	vertices.push_back(A);

	B.Position = vertex2;
	B.Normal = normal0;

	vertices.push_back(B);

	C.Position = vertex3;
	C.Normal = normal0;

	vertices.push_back(C);

	AA.Position = vertex0;
	AA.Normal = normal1;

	vertices.push_back(AA);

	BB.Position = vertex2;
	BB.Normal = normal1;

	vertices.push_back(BB);

	CC.Position = vertex1;
	CC.Normal = normal1;

	vertices.push_back(CC);

	AAA.Position = vertex0;
	AAA.Normal = normal2;

	vertices.push_back(AAA);

	BBB.Position = vertex3;
	B.Normal = normal2;

	vertices.push_back(BBB);

	CCC.Position = vertex2;
	C.Normal = normal2;

	vertices.push_back(CCC);

	AAAA.Position = vertex1;
	AAAA.Normal = normal3;

	vertices.push_back(AAAA);

	BBBB.Position = vertex3;
	BBBB.Normal = normal3;

	vertices.push_back(BBBB);

	CCCC.Position = vertex0;
	CCCC.Normal = normal3;

	vertices.push_back(CCCC);

	
	//Create the vertex indices
	vector<GLuint> indices = { 0, 1, 2,
								3, 4, 5,
								6, 7, 8,
								9, 10, 11 };
	
	//create the mesh
	Mesh myMesh = Mesh(vertices, indices);

	// World space positions of our cones
	glm::vec3 positions[] = 
	{
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	
	// Draw loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated
		glfwPollEvents();
		do_Movement();

		// Render
		// Clear the colorbuffer
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use ourshader
		ourShader.Use();
		GLint objectColorLoc = glGetUniformLocation(ourShader.Program, "objectColor");
		GLint lightPosLoc = glGetUniformLocation(ourShader.Program, "light.position");
		GLint viewPosLoc = glGetUniformLocation(ourShader.Program, "viewPos");
		
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.5f);
	
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		// Set lights properties
		glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.75f); // Decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.50f); // Low influence
		glUniform3f(glGetUniformLocation(ourShader.Program, "light.ambient"), ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform3f(glGetUniformLocation(ourShader.Program, "light.diffuse"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(glGetUniformLocation(ourShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		
		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
		
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//Draw the mesh
		for (GLuint i = 0; i < 6; i++)
		{
			// Calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model;
			model = glm::translate(model, positions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			myMesh.Draw(ourShader);
		}
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Moves/alters the camera positions based on user input
void do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//Escape closes the program
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if(key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}