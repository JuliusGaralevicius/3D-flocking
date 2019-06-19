// GLEW


#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Vector3.h"
#include "Matrix4.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "Boids.h"
#include "Shader.h"
#include <string>
#include "DebugShape.h"
#include "Obstacles.h"

using namespace std;

void initglfw();
void initglew();
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void getinput(GLFWwindow* window);
void moveGoal(float dt);

// Window dimensions
GLuint WIDTH = 1920, HEIGHT = 1080;

static const float cameraSpeed = 300.0f;
static const float cameraRotationSpeed = 4.0f;
static const float goalMoveSpeed = 30.0f;
static const bool scripted = true;

// how much the camera should move during this frame
Vector3 cameraTranslate(0, 0, 0);

// paths of different types of boids
vector<Vector4> goalLocations1 = { Vector4(256, 256, 400, 0), Vector4(400, 256, 256, 0), Vector4(256, 256, 100, 0), Vector4(100, 256, 256, 0) };
vector<Vector4> goalLocations2 = { Vector4(100, 256, 256, 0) , Vector4(256, 256, 100, 0), Vector4(400, 256, 256, 0), Vector4(256, 256, 400, 0) };
Vector4 currGoalLocation1 = goalLocations1[0];
int currentGoal1 = 1;
Vector4 currGoalLocation2 = goalLocations2[0];
int currentGoal2 = 1;

// view matrix centered to Boids flock
Matrix4 view = Matrix4::Translation(Vector3(-256, -256, -900));

// camera focus position
Vector3 cameraTarget(256.0, 256.0, 256.0);

Matrix4 projection = Matrix4::Perspective(30.0, 3000.0f, (float)WIDTH/ (float)HEIGHT, 60.0f);

float avoidance = 4.0f;
float homing = 0.000000005f;

Vector4 screenColour(255.0f, 244.0f, 230.0f, 1.0f);
Vector4 boidColour1(133.0f, 68.0f, 66.0f, 1.0f);
Vector4 boidColour2(60.0f, 47.0f, 47.0f, 1.0f);

// must be a multiple of 512
int flocksize = 512*8;

// flocksize*multiplier size flock will be created when behaviour is scripted
int multiplier = 1;

// scripted behaviour controllers
float elapsedTime = 0.0f;
float changeFlocksizeInterval = 10.0f;

// previous screen position
double xpPos = 0.0;
double ypPos = 0.0;

// is the flock avoiding obstacles?
bool bAvoidObstacles = false;

// the last state of right mouse click. Used to set previous screen position correctly to avoid jumps when rotating camera
int rButtonState = GLFW_RELEASE;

// render based on flags
enum RENDERFLAGS
{
	BOIDS = 0,
	OBSTACLES = 1,
	DEBUG = 2
};
bool renderFlags[3] = { 1, 0, 0 };

Boids* b = nullptr;

int main()
{
	initglfw();
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Title", NULL, NULL);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glViewport(0, 0, WIDTH, HEIGHT);

	initglew();

	double previousTime= glfwGetTime();
	double currentTime = previousTime;
	double dt = 0;

	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(0);

	Obstacles* obs = new Obstacles();

	b = new Boids(obs, flocksize, boidColour1, boidColour2);
	b->setParams(avoidance, homing);

	DebugShape *grid = new DebugShape(false, b->atomicCounter);

	for (unsigned long int i = 0; !glfwWindowShouldClose(window); i++) {

		currentTime = glfwGetTime();
		dt = currentTime - previousTime;
		elapsedTime += dt;
		previousTime = currentTime;
		
		if (scripted && elapsedTime >= changeFlocksizeInterval && multiplier<=10)
		{
			delete b;
			delete grid;

			elapsedTime = 0.0f;
			multiplier++;

			if (multiplier == 3)
			{
				renderFlags[OBSTACLES] = true;
				bAvoidObstacles = true;
			}
			if (multiplier == 6)
			{
				renderFlags[OBSTACLES] = false;
				bAvoidObstacles = false;
				renderFlags[DEBUG] = true;
			}
			if (multiplier == 8)
			{
				renderFlags[OBSTACLES] = true;
				bAvoidObstacles = true;
				renderFlags[DEBUG] = false;
			}

			b = new Boids(obs, flocksize*multiplier, boidColour1, boidColour2);
			b->setParams(avoidance, homing);
			b->setAvoid(bAvoidObstacles);
		
			grid = new DebugShape(false, b->atomicCounter);
		}

		glfwPollEvents();
		getinput(window);
		view = view*Matrix4::Translation(cameraTranslate*cameraSpeed*dt);

		if (!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS))
			view = view * Matrix4::Translation(cameraTarget) *Matrix4::Rotation(dt*cameraRotationSpeed, Vector3(0, 1, 0)) * Matrix4::Translation(cameraTarget.Inverse());

		Matrix4 vp = projection * view;

		if (i % 30 == 0)
			glfwSetWindowTitle(window, (to_string(flocksize*multiplier) + " Boids running at " + to_string(1 / dt) + " FPS ").c_str() );

		if (renderFlags[BOIDS])
			b->render(dt, currGoalLocation1, currGoalLocation2, vp);
		if (renderFlags[OBSTACLES])
			obs->Render(dt, vp, currentTime);
		if (renderFlags[DEBUG])
			grid->render(vp, dt);

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
		renderFlags[BOIDS] = !renderFlags[BOIDS];
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		renderFlags[OBSTACLES] = !renderFlags[OBSTACLES];
		bAvoidObstacles = !bAvoidObstacles;
		b->setAvoid(bAvoidObstacles);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		renderFlags[DEBUG] = !renderFlags[DEBUG];
	}
}
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (rButtonState != GLFW_PRESS)
		{
			xpPos = xpos;
			ypPos = ypos;
			rButtonState = GLFW_PRESS;
		}

		float deltaX = xpPos - xpos;
		float deltaY = ypPos - ypos;

		view = view * Matrix4::Translation(cameraTarget) *Matrix4::Rotation(deltaX*cameraRotationSpeed*0.25, Vector3(0, 1, 0)) * Matrix4::Translation(cameraTarget.Inverse());

		xpPos = xpos;
		ypPos = ypos;
	}

}
void initglfw() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		rButtonState = GLFW_RELEASE;
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


	// control behaviour of Boids

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && avoidance < 30)
	{
		avoidance += 0.05;
		b->setParams(avoidance, homing);
		std::cout << "Avoidance: " << avoidance << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && avoidance > 0.05)
	{
		avoidance -= 0.05;
		b->setParams(avoidance, homing);
		std::cout << "Avoidance: " << avoidance << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && homing > 0.0000000000000005f)
	{
		homing /= 1.05;
		b->setParams(avoidance, homing);
		std::cout << "homing: " << homing << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && homing < 0.05f)
	{
		homing *= 1.05;

		b->setParams(avoidance, homing);
		std::cout << "homing: " << homing << std::endl;
	}
}
void initglew() {
	GLenum error = glewInit();
}