#pragma once
#include <vector>
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <GL/glew.h>
class Obstacles;
class Boid
{
	
	// layout in the shader
	enum buffers {
		POSITION = 0,
		COLOUR = 1,
		NORMAL = 2,
		MAX_BUFFER = 3
	};
private:
	std::vector<Vector4> colours = {
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),

		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),

		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),

		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),

		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),

		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f),
		Vector4(0.5f, 0.5f, 0.5f, 1.0f)
	};
	std::vector<Vector4> positions = {
		Vector4(1, -1, -1, 1.0f),
		Vector4(-1, -1, -1, 1.0f),
		Vector4(0, 0, 1, 1.0f),

		Vector4(-1, -1, -1, 1.0f),
		Vector4(-1, -1, 1, 1.0f),
		Vector4(0, 0, 1, 1.0f),

		Vector4(-1, -1, 1, 1.0f),
		Vector4(1, -1, 1, 1.0f),
		Vector4(0, 0, 1, 1.0f),

		Vector4(1, -1, 1, 1.0f),
		Vector4(1, -1, -1, 1.0f),
		Vector4(0, 0, 1, 1.0f),

		Vector4(1, -1, -1, 1.0f),
		Vector4(-1, -1, -1, 1.0f),
		Vector4(-1, -1, 1, 1.0f),

		Vector4(-1, -1, 1, 1.0f),
		Vector4(1, -1, 1, 1.0f),
		Vector4(1, -1, -1, 1.0f)
	};
	unsigned static const int CELLCOUNTDIMENSION = 16; // ^3 = total cells
	struct boid_data_t
	{
		float position[4];
		float velocity[3];
		float type;
	};
	GLuint renderVAO[2];
	GLuint sortVAO;
	GLuint ssboBoids[2];
	GLuint sumSSBO;
	GLuint sumSSBOOG;
	GLuint obstacleSSBO;

	GLuint maxSSBO;
	Obstacles* obs;

	bool odd = false;

	GLuint computePositions;
	GLuint computeHash;
	GLuint computeScan;
	GLuint computeCombine;
	GLuint computeSort;


	GLuint IDShader;
	GLuint mvpLoc;
	GLuint goalLoc[2] = { 0, 0 };
	GLuint dtLoc;

	GLuint avoidanceLoc;
	GLuint homingLoc;
	GLuint avoidObstaclesBool;

	GLuint col1;
	GLuint col2;

	Vector4 colour1;
	Vector4 colour2;
	
public:
	GLuint atomicCounter;
	float avoidance = 4;
	// should the flock avoid obstacles
	float avoid = 0.0f;
	float homingValue = 6;

	void toggleAvoidance();
	void setParams(float avoidance, float homing);
	unsigned const int FLOCKSIZE;
	GLuint emptyArr[CELLCOUNTDIMENSION*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION] = { 0 };
	Matrix4 model = Matrix4::Translation(Vector3(0, 0, 0));
	Boid(Obstacles* obs, int size, Vector4 colour1, Vector4 colour2);
	void render(double deltaTime, Vector4 goal1, Vector4 goal2, Matrix4 vp);
	std::vector<Vector4> getPositions() { return positions; };
	GLuint getRenderProgram() { return IDShader; };
	~Boid();
};