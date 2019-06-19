/*
Class: Boids
Author: Julius Garalevicius
Description: Class responsible for drawing a grid of cubes visualising the density of Boids.
*/
#pragma once
#include <vector>
#include "Vector4.h"
#include "Matrix4.h"
#include <GL/glew.h>
class DebugShape
{
public:
	DebugShape(bool big, GLuint boidCount);
	~DebugShape();

	const GLuint numCells = 16;
	const float gridDimension = 512.0f;
	const float cellDimension = gridDimension / (float)numCells;
	GLuint index = 0;

	std::vector<Vector4> positions
	{
		// back
		Vector4(0, 0.0, -cellDimension, 1.0f),
		Vector4(cellDimension, 0.0, -cellDimension, 1.0f),
		
		Vector4(cellDimension, 0.0, -cellDimension, 1.0f),
		Vector4(cellDimension, cellDimension, -cellDimension, 1.0f),
		
		Vector4(cellDimension, cellDimension, -cellDimension, 1.0f),
		Vector4(0.0, cellDimension, -cellDimension, 1.0f),
		
		Vector4(0.0, cellDimension, -cellDimension, 1.0f),
		Vector4(0.0, 0.0, -cellDimension, 1.0f),
		
		// sides
		Vector4(0.0, 0.0, -cellDimension, 1.0f),
		Vector4(0.0, 0.0, 0.0f, 1.0f),
		
		Vector4(cellDimension, 0.0, -cellDimension, 1.0f),
		Vector4(cellDimension, 0.0, 0.0f, 1.0f),
		
		Vector4(cellDimension, cellDimension, -cellDimension, 1.0f),
		Vector4(cellDimension, cellDimension, 0.0f, 1.0f),
		
		Vector4(0.0, cellDimension, -cellDimension, 1.0f),
		Vector4(0.0, cellDimension, 0.0f, 1.0f),
		
		// front
		Vector4(0, 0.0, 0.0, 1.0f),
		Vector4(cellDimension, 0.0, 0.0, 1.0f),
		
		Vector4(cellDimension, 0.0, 0.0, 1.0f),
		Vector4(cellDimension, cellDimension, 0.0, 1.0f),
		
		Vector4(cellDimension, cellDimension, 0.0, 1.0f),
		Vector4(0.0, cellDimension, 0.0, 1.0f),
		
		Vector4(0.0, cellDimension, 0.0, 1.0f),
		Vector4(0.0, 0.0, 0.0, 1.0f)
	};
	void render(Matrix4 vp, float dt);

	Matrix4 model = Matrix4::Translation(Vector3(0.0, 0.0, 0.0));

	// boid count in each bin
	GLuint counter;

	GLuint posVBO;
	GLuint vao;
	GLuint renderProgram;
	GLuint mvpLoc;
};
