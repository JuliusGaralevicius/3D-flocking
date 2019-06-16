#pragma once
#include <vector>
#include <GL/glew.h>
#include "Vector3.h"
#include "Matrix4.h"
class Obstacles
{
public:
	std::vector<Vector3> geometry;
	struct obstacle_data_t
	{
		float position[3];
		float radius;
		float velocity[4];
	};


	GLuint renderProgram;
	GLuint computeProgram;
	GLuint renderVAO;

	GLuint obstacleSSBO;

	GLuint mvpLoc;
	GLuint vpLoc;
	GLuint mLoc;

	GLuint t;
	GLuint deltaT;

	unsigned static const int NUMSIDES = 32;
	unsigned static const int OBSTACLESCOUNT = 16;
	Matrix4 model = Matrix4::Translation(Vector3(0, 0, 0));

	float cellDimension = 0.5f;
	std::vector<Vector4> positions
	{
		// back
		Vector4(-cellDimension , -cellDimension, -cellDimension, 1.0f),
		Vector4(cellDimension, -cellDimension, -cellDimension, 1.0f),
		Vector4(cellDimension, cellDimension, -cellDimension, 1.0f),


		Vector4(cellDimension, cellDimension, -cellDimension, 1.0f),
		Vector4(-cellDimension, cellDimension, -cellDimension, 1.0f),
		Vector4(-cellDimension , -cellDimension, -cellDimension, 1.0f),

		// left
		Vector4(-cellDimension , -cellDimension, -cellDimension, 1.0f),
		Vector4(-cellDimension , -cellDimension, cellDimension, 1.0f),
		Vector4(-cellDimension , cellDimension, cellDimension, 1.0f),

		Vector4(-cellDimension , cellDimension, cellDimension, 1.0f),
		Vector4(-cellDimension , cellDimension, -cellDimension, 1.0f),
		Vector4(-cellDimension , -cellDimension, -cellDimension, 1.0f),

		// right

		Vector4(cellDimension, -cellDimension, -cellDimension, 1.0f),
		Vector4(cellDimension, -cellDimension, cellDimension, 1.0f),
		Vector4(cellDimension, cellDimension, cellDimension, 1.0f),

		Vector4(cellDimension, cellDimension, cellDimension, 1.0f),
		Vector4(cellDimension, cellDimension, -cellDimension, 1.0f),
		Vector4(cellDimension, -cellDimension, -cellDimension, 1.0f),


		// top
		Vector4(cellDimension, cellDimension, -cellDimension, 1.0f),
		Vector4(cellDimension, cellDimension, cellDimension, 1.0f),
		Vector4(-cellDimension, cellDimension, cellDimension, 1.0f),

		Vector4(-cellDimension, cellDimension, cellDimension, 1.0f),
		Vector4(-cellDimension, cellDimension, -cellDimension, 1.0f),
		Vector4(cellDimension, cellDimension, -cellDimension, 1.0f),

		// bottom 
		Vector4(-cellDimension, -cellDimension, -cellDimension, 1.0f),
		Vector4(cellDimension, -cellDimension, -cellDimension, 1.0f),
		Vector4(cellDimension, -cellDimension, cellDimension, 1.0f),

		Vector4(cellDimension, -cellDimension, cellDimension, 1.0f),
		Vector4(-cellDimension, -cellDimension, cellDimension, 1.0f),
		Vector4(-cellDimension, -cellDimension, -cellDimension, 1.0f),

		// front
		Vector4(-cellDimension , -cellDimension, cellDimension, 1.0f),
		Vector4(cellDimension, -cellDimension, cellDimension, 1.0f),
		Vector4(cellDimension, cellDimension, cellDimension, 1.0f),

		Vector4(cellDimension, cellDimension, cellDimension, 1.0f),
		Vector4(-cellDimension, cellDimension, cellDimension, 1.0f),
		Vector4(-cellDimension , -cellDimension, cellDimension, 1.0f)
	};


	void Render(double dt, Matrix4 vp, float time);

	Obstacles();
	~Obstacles();
};

