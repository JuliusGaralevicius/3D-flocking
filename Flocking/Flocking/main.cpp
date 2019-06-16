// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include "Vector3.h"
#include "Matrix4.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "Boid.h"
#include "Shader.h"
#include <string>
#include "DebugShape.h"
#include "Obstacles.h"

using namespace std;

// Window dimensions
GLuint WIDTH = 1920, HEIGHT = 1080;

unsigned int currentShader = 0;
static const float cameraSpeed = 300.0f;
static const float goalMoveSpeed = 30.0f;



int main();

void initglfw();
void initglew();
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void getinput(GLFWwindow* window);
void moveGoal(float dt);
Vector3 cameraTranslate(0, 0, 0);


std::vector<Vector4> goalLocations1 = { Vector4(256, 256, 400, 0), Vector4(400, 256, 256, 0), Vector4(256, 256, 100, 0), Vector4(100, 256, 256, 0) };
std::vector<Vector4> goalLocations2 = { Vector4(100, 256, 256, 0) , Vector4(256, 256, 100, 0), Vector4(400, 256, 256, 0), Vector4(256, 256, 400, 0) };
Vector4 currGoalLocation1 = goalLocations1[0];
int currentGoal1 = 1;
Vector4 currGoalLocation2 = goalLocations2[0];
int currentGoal2 = 1;

Matrix4 view = Matrix4::Translation(Vector3(-256, -256, -1024));
Vector3 cameraTarget(-256.0, -256.0, 0.0);
Vector3 upVec(0.0, 1.0, 0.0);
Vector3 rightVec(1.0, 0.0, 0.0);

Matrix4 projection = Matrix4::Perspective(30.0, 3000.0f, (float)WIDTH/ (float)HEIGHT, 60.0f);

float avoidance = 3.0f;
float homing = 0.000000005f;

Vector4 screenColour(255.0f, 244.0f, 230.0f, 1.0f);
Vector4 boidColour1(133.0f, 68.0f, 66.0f, 1.0f);
Vector4 boidColour2(60.0f, 47.0f, 47.0f, 1.0f);

int flocksize = 40000;
int multiplier = 1;
float elapsedTime = 0.0f;
float changeFlocksizeInterval = 5.0f;

double xpPos = 0.0;
double ypPos = 0.0;

bool avoidObstacles = false;

bool renderFlags[3] = { 1, 1, 0 };

enum RENDERFLAGS
{
	BOIDS = 0,
	OBSTACLES = 1,
	DEBUG = 2
};

Boid* b;

int main()
{



	initglfw();
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Title", NULL, NULL);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glViewport(0, 0, WIDTH, HEIGHT);

	initglew();
	Obstacles* obs = new Obstacles();




	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	double previousTime= glfwGetTime();
	double currentTime = glfwGetTime();
	double dt = 0;
	glEnable(GL_DEPTH_TEST);

	glfwSwapInterval(0);

	b = new Boid(obs, flocksize, boidColour1, boidColour2);
	b->toggleAvoidance();
	DebugShape *grid = new DebugShape(false, b->atomicCounter);
	std::vector<double> fps;
	for (unsigned long int i = 0; !glfwWindowShouldClose(window); i++) {

		currentTime = glfwGetTime();
		dt = currentTime - previousTime;
		elapsedTime += dt;
		//if (elapsedTime >= changeFlocksizeInterval && multiplier<=10)
		//{
		//	//delete b;
		//	//delete grid;
		//	elapsedTime = 0.0f;
		//	multiplier++;
		//	//b = new Boid(obs, flocksize*multiplier, boidColour1, boidColour2);
		//	if (multiplier == 3)
		//	{
		//		renderFlags[OBSTACLES] = true;
		//		b->toggleAvoidance();
		//		avoidObstacles = true;
		//	}
		//	if (multiplier == 6)
		//	{
		//		renderFlags[OBSTACLES] = false;
		//		avoidObstacles = false;
		//		b->toggleAvoidance();
		//		renderFlags[DEBUG] = true;
		//	}
		//	if (multiplier == 8)
		//	{
		//		renderFlags[OBSTACLES] = true;
		//		avoidObstacles = true;
		//		b->toggleAvoidance();
		//		renderFlags[DEBUG] = false;
		//	}
		//
		//	//grid = new DebugShape(false, b->atomicCounter);
		//}

		glfwPollEvents();
		getinput(window);
		view = view*Matrix4::Translation(cameraTranslate*cameraSpeed*dt);

		view = view * Matrix4::Translation(Vector3(256.0, 256, 256));

		view = view * Matrix4::Rotation(dt*4, Vector3(0, 1, 0));

		view = view * Matrix4::Translation(Vector3(-256.0, -256, -256));


		Matrix4 vp = projection * view;
		previousTime = currentTime;
		
		if (i % 60 == 0)
		glfwSetWindowTitle(window, std::to_string(1 / dt).c_str());


		b->setParams(avoidance, homing);
		if (renderFlags[0])
		b->render(dt, currGoalLocation1, currGoalLocation2, vp);

		if (renderFlags[1])
		obs->Render(dt, vp, currentTime);

		if (renderFlags[2])
		grid->render(vp, dt);

		fps.push_back(1 / dt);
		moveGoal(dt);
		cameraTranslate.ToZero();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(screenColour.x / 255.0f, screenColour.y / 255.0f, screenColour.z/255.0f, screenColour.w);


	}

	glfwTerminate();
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{	
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		renderFlags[0] = !renderFlags[0];
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		renderFlags[1] = !renderFlags[1];
		avoidObstacles = !avoidObstacles;
		b->toggleAvoidance();
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		renderFlags[2] = !renderFlags[2];
	}
}
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{

	float deltaX = xpPos - xpos;
	float deltaY = ypPos - ypos;

	view = view*Matrix4::Translation(Vector3(256.0, 256, 256));
	
	view = view * Matrix4::Rotation(deltaX*0.5, Vector3(0, 1, 0));

	view = view * Matrix4::Translation(Vector3(-256.0, -256, -256));

	xpPos = xpos;
	ypPos = ypos;
}
void initglfw() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);
		xpos = (xpos / WIDTH) * 2 - 1;
		ypos = -((ypos / HEIGHT) * 2 - 1);
		currGoalLocation1 = Vector4(xpos * 150, ypos * 150, 0, 0);
	}
}
void moveGoal(float dt)
{
	Vector4 aimLocation1 = goalLocations1[currentGoal1];
	Vector4 direction1 = (aimLocation1 - currGoalLocation1).normalize();
	currGoalLocation1 += direction1 * goalMoveSpeed * dt;

	Vector4 aimLocation2 = goalLocations2[currentGoal2];
	Vector4 direction2 = (aimLocation2 - currGoalLocation2).normalize();
	currGoalLocation2 += direction2 * goalMoveSpeed * dt;

	if ((aimLocation1 - currGoalLocation1).Lenght() <= 10.0f)
	{
		if (currentGoal1 < goalLocations1.size() - 1)
		{
			currentGoal1++;
		}
		else {
			currentGoal1 = 0;
		}
	}
	if ((aimLocation2 - currGoalLocation2).Lenght() <= 10.0f)
	{
		if (currentGoal2 < goalLocations2.size() - 1)
		{
			currentGoal2++;
		}
		else {
			currentGoal2 = 0;
		}
	}
}
void getinput(GLFWwindow* window)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		std::cout << view.lookAtVector() << std::endl;

		x = view.lookAtVector().x;
		y = view.lookAtVector().y;
		z = view.lookAtVector().z;

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		x = view.lookAtVector().Inverse().x;
		y = view.lookAtVector().Inverse().y;
		z = view.lookAtVector().Inverse().z;
	}

	cameraTranslate = Vector3(x, y, z);

	//if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	//{
	//	avoidance += 0.05;
	//	std::cout << "Avoidance: " << avoidance << std::endl;
	//}
	//if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	//{
	//	avoidance -= 0.05;
	//	std::cout << "Avoidance: " << avoidance << std::endl;
	//}
	//if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	//{
	//	otherAvoidance *= 1.25;
	//	std::cout << "homing: " << otherAvoidance << std::endl;
	//}
	//if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	//{
	//	otherAvoidance /= 1.25;
	//	std::cout << "homing: " << otherAvoidance << std::endl;
	//}
}
void initglew() {
	GLenum error = glewInit();
}