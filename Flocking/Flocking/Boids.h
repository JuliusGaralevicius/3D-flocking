/*
Class: Boids
Author: Julius Garalevicius
Description: Class responsible for simulating Boid behaviour, spatial subdivision and rendering Boids.
*/
#pragma once
#include <vector>
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <GL/glew.h>
class Obstacles;
struct boid_data_t
{
	float position[4];
	float velocity[3];
	float type;
};
class Boids
{
private:
	unsigned static const int LOCAL_GROUP_SIZE = 512;
	unsigned static const int CELL_COUNT_1D = 16; // ^3 = total cells
	unsigned const int FLOCKSIZE;


	std::vector<Vector4> positions = {
		Vector4(1, -1, -1, 1.0f),
		Vector4(-1, -1, -1, 1.0f),
		Vector4(0, 1, 0, 1.0f),

		Vector4(-1, -1, -1, 1.0f),
		Vector4(-1, -1, 1, 1.0f),
		Vector4(0, 1, 0, 1.0f),

		Vector4(-1, -1, 1, 1.0f),
		Vector4(1, -1, 1, 1.0f),
		Vector4(0, 1, 0, 1.0f),

		Vector4(1, -1, 1, 1.0f),
		Vector4(1, -1, -1, 1.0f),
		Vector4(0, 1, 0, 1.0f),

		Vector4(1, -1, -1, 1.0f),
		Vector4(-1, -1, -1, 1.0f),
		Vector4(-1, -1, 1, 1.0f),

		Vector4(-1, -1, 1, 1.0f),
		Vector4(1, -1, 1, 1.0f),
		Vector4(1, -1, -1, 1.0f)
	};
	
	// buffers for computing
	GLuint ssboBoids[2];
	GLuint sumSSBO;
	GLuint sumSSBOOG;
	GLuint maxSSBO;

	// render VAO's even and odd
	GLuint renderVAO[2];

	// Obstacles to avoid
	Obstacles* obs = nullptr;

	// odd or even frames
	bool odd = false;


	// Compute Shaders
	GLuint computePositions;
	GLuint computeHash;
	GLuint computeScan;
	GLuint computeCombine;
	GLuint computeSort;

	// Render program
	GLuint renderProgram;

	// uniforms for rendering
	GLuint mvpLoc;
	GLuint goalLoc[2] = { 0, 0 };
	GLuint dtLoc;

	// uniforms for boid behaviour
	GLuint avoidanceLoc;
	GLuint homingLoc;
	GLuint bAvoidObstacles;

	// values controling behaviour
	float avoidanceStrength = 4.f;
	float homingStrength = 6.f;
	float bAvoid = 0.f;

	// Colours of boids (uniforms and actual colours)
	GLuint col1;
	GLuint col2;

	// to reset the counting
	GLuint emptyArr[CELL_COUNT_1D*CELL_COUNT_1D*CELL_COUNT_1D] = { 0 };

	void setupShaders();
	void findUniforms();
	boid_data_t* initializeFlock(GLuint size);
public:

	// used by compute shaders and by DebugShape to visualise density
	GLuint atomicCounter;

	// should avoid obstacles?
	void setAvoid(bool avoid);

	void setParams(float avoidance, float homing);

	
	Matrix4 model = Matrix4::Translation(Vector3(1, 1, 1));

	Boids(Obstacles* obs, int size, Vector4 colour1, Vector4 colour2);

	void render(double deltaTime, Vector4 goal1, Vector4 goal2, Matrix4 vp);

	void setColour(Vector4 colour1, Vector4 colour2);

	~Boids();
};