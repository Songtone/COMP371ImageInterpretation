//example skeleton code
//modified from http://learnopengl.com/

#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "objloader.hpp"  //include the object loader
#include "CImg.h"
#include "camera.h"

using namespace std;
using namespace cimg_library;

// Window dimensions
const GLuint WIDTH = 1200, HEIGHT = 1000;

glm::vec3 camera_position;
glm::vec3 triangle_scale;
glm::mat4 projection_matrix;

// camera
Camera camera(glm::vec3(150.0f, 300.0f, 100.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//variables
int skipSize;
int skipOption = 1;
int skipAgain = 0;
int polygonShape = 0;
float cameraX = 550.0f;
float cameraY = 520.0f;
float cameraZ = 530.0f;
float panning = 0.0f;

// Constant vectors
const glm::vec3 center(0.0f, 0.0f, 0.0f);
const glm::vec3 up(0.0f, 1.0f, 0.0f);
const glm::vec3 eye(0.0f, 0.0f, 10.0f);

//functions used
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);


// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Load one cube", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 1000.0f);

	// Build and compile our shader program
	// Vertex shader

	// Read the Vertex Shader code from the file
	string vertex_shader_path = "vertex.shader";
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, ios::in);

	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_path = "fragment.shader.shader";
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	//Show the picture that will be used
	CImg<float> picture("depth.bmp");
	CImgDisplay main_disp(picture, "The Depth Picture");

	cout << picture.size() << endl;
	//Pixel Data


	//iteration through the picture pixels without skips
	vector <glm::vec3> pictureData;
	vector <glm::vec3> pictureDataHigh;
	vector <glm::vec3> pictureDataMedium;
	vector <glm::vec3> pictureDataLow;
	//cout << "What is the skip-size desired?" << endl;
	//cin >> skipSize;

	for (int x = (0 - picture.width() / 2); x < (picture.width()) / 2; x++) {
		for (int z = (0 - picture.height() / 2); z < (picture.height()) / 2; z++) {
			float height = static_cast<float>(*picture.data(x + (picture.width() / 2), z + (picture.height() / 2)));
			pictureData.emplace_back(glm::vec3(x, height, z));
			
			
		}
	}


	GLuint VAO_pic, VBO_pic/*, VBO_pic_high, VBO_pic_medium, VBO_pic_low*/;
	glGenVertexArrays(1, &VAO_pic);
	glGenBuffers(1, &VBO_pic);
	

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO_pic);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_pic);
	glBufferData(GL_ARRAY_BUFFER, pictureData.size() * sizeof(glm::vec3), &pictureData.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO



	//iteration through the picture pixels with skips
	vector <glm::vec3> pictureDataSkip;
	cout << "What is the skip-size desired?" << endl;
	cin >> skipSize;

	for (int x = (0 - picture.width() / 2); x < (picture.width()) / 2; x += skipSize) {
		for (int z = (0 - picture.height() / 2); z < (picture.height()) / 2; z += skipSize) {
			float height = static_cast<float>(*picture.data(x + (picture.width() / 2), z + (picture.height() / 2)));
			pictureDataSkip.emplace_back(glm::vec3(x, height, z));
		}
	}

	GLuint VAO_skipSize, VBO_skipSize;
	glGenVertexArrays(1, &VAO_skipSize);
	glGenBuffers(1, &VBO_skipSize);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO_skipSize);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_skipSize);
	glBufferData(GL_ARRAY_BUFFER, pictureDataSkip.size() * sizeof(glm::vec3), &pictureDataSkip.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO


	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");
	GLuint object_type_loc = glGetUniformLocation(shaderProgram, "object_type");

	glEnable(GL_BLEND | GL_DEPTH_TEST);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		if (skipAgain == 1) {//to ask the user a skip size while program is running
			pictureDataSkip.clear();
			cout << "What is the skip-size desired?" << endl;
			cin >> skipSize;

			for (int x = (0 - picture.width() / 2); x < (picture.width()) / 2; x += skipSize) {
				for (int z = (0 - picture.height() / 2); z < (picture.height()) / 2; z += skipSize) {
					float height = static_cast<float>(*picture.data(x + (picture.width() / 2), z + (picture.height() / 2)));
					pictureDataSkip.emplace_back(glm::vec3(x, height, z));
				}
			}

			GLuint VAO_skipSize, VBO_skipSize;
			glGenVertexArrays(1, &VAO_skipSize);
			glGenBuffers(1, &VBO_skipSize);

			// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
			glBindVertexArray(VAO_skipSize);

			glBindBuffer(GL_ARRAY_BUFFER, VBO_skipSize);
			glBufferData(GL_ARRAY_BUFFER, pictureDataSkip.size() * sizeof(glm::vec3), &pictureDataSkip.front(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(0);
			skipAgain = 0;// Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
		}

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();

		glm::mat4 model_matrix;


		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));


		if (skipOption == 0) {
			
			glBindVertexArray(VAO_pic);
			if (polygonShape == 0) {
				glDrawArrays(GL_POINTS, 0, pictureData.size());
			}
			if (polygonShape == 1) {
				glDrawArrays(GL_TRIANGLES, 0, pictureData.size());
			}
			glBindVertexArray(0);
		}
		if (skipOption == 1) {
			
			glBindVertexArray(VAO_skipSize);
			if (polygonShape == 0) {
				glDrawArrays(GL_POINTS, 0, pictureDataSkip.size());
			}
			if (polygonShape == 1) {
				glDrawArrays(GL_TRIANGLES, 0, pictureDataSkip.size());
			}
			glBindVertexArray(0);
		}



		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;

	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		cameraY = cameraY + 10.0f;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		cameraY = cameraY - 10.0f;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		cameraX = cameraX + 10.0f;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		cameraX = cameraX - 10.0f;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		cameraZ = cameraZ + 10.0f;
	}
	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
		cameraZ = cameraZ - 10.0f;
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		skipOption = 0;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		skipOption = 1;
	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		polygonShape = 0;
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		polygonShape = 1;
	}
	if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
		skipOption = 0;
		skipAgain = 1;
		polygonShape = 0;
		cameraX = 550.0f;
		cameraY = 520.0f;
		cameraZ = 530.0f;

	}

}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}
