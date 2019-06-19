/*
Class: Boids
Author: Julius Garalevicius
Description: Class responsible moving and rendering the Obstacles for Boids.
*/
#pragma once
#include <vector>
#include <GL/glew.h>
#include "Matrix4.h"
class Obstacles
{
public:
	struct obstacle_data_t
	{
		float position[3];
		float radius;
		float velocity[4];
	};

	void Render(double dt, Matrix4 vp, float time);
	GLuint obstacleSSBO;

	Obstacles();
	~Obstacles();

private:
	unsigned const int OBSTACLE_COUNT = 16;
	unsigned const int RADIUS_LIMITS[2] = { 20, 50 };

	GLuint renderProgram;
	GLuint computeProgram;

	GLuint renderVAO;

	// uniforms
	GLuint mvpLoc;
	GLuint vpLoc;
	GLuint mLoc;
	GLuint deltaT;


	Matrix4 model = Matrix4::Translation(Vector3(0, 0, 0));

	const float HALF = 0.5f;
	std::vector<Vector4> positions
	{
		// back
		Vector4(-HALF , -HALF, -HALF, 1.0f),
		Vector4(HALF, -HALF, -HALF, 1.0f),
		Vector4(HALF, HALF, -HALF, 1.0f),


		Vector4(HALF, HALF, -HALF, 1.0f),
		Vector4(-HALF, HALF, -HALF, 1.0f),
		Vector4(-HALF , -HALF, -HALF, 1.0f),

		// left
		Vector4(-HALF , -HALF, -HALF, 1.0f),
		Vector4(-HALF , -HALF, HALF, 1.0f),
		Vector4(-HALF , HALF, HALF, 1.0f),

		Vector4(-HALF , HALF, HALF, 1.0f),
		Vector4(-HALF , HALF, -HALF, 1.0f),
		Vector4(-HALF , -HALF, -HALF, 1.0f),

		// right

		Vector4(HALF, -HALF, -HALF, 1.0f),
		Vector4(HALF, -HALF, HALF, 1.0f),
		Vector4(HALF, HALF, HALF, 1.0f),

		Vector4(HALF, HALF, HALF, 1.0f),
		Vector4(HALF, HALF, -HALF, 1.0f),
		Vector4(HALF, -HALF, -HALF, 1.0f),


		// top
		Vector4(HALF, HALF, -HALF, 1.0f),
		Vector4(HALF, HALF, HALF, 1.0f),
		Vector4(-HALF, HALF, HALF, 1.0f),

		Vector4(-HALF, HALF, HALF, 1.0f),
		Vector4(-HALF, HALF, -HALF, 1.0f),
		Vector4(HALF, HALF, -HALF, 1.0f),

		// bottom 
		Vector4(-HALF, -HALF, -HALF, 1.0f),
		Vector4(HALF, -HALF, -HALF, 1.0f),
		Vector4(HALF, -HALF, HALF, 1.0f),

		Vector4(HALF, -HALF, HALF, 1.0f),
		Vector4(-HALF, -HALF, HALF, 1.0f),
		Vector4(-HALF, -HALF, -HALF, 1.0f),

		// front
		Vector4(-HALF , -HALF, HALF, 1.0f),
		Vector4(HALF, -HALF, HALF, 1.0f),
		Vector4(HALF, HALF, HALF, 1.0f),

		Vector4(HALF, HALF, HALF, 1.0f),
		Vector4(-HALF, HALF, HALF, 1.0f),
		Vector4(-HALF , -HALF, HALF, 1.0f)
	};

};

