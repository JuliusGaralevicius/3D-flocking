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

	GLuint numCells = 16;
	float gridDimension = 512.0f;
	float cellDimension = gridDimension / (float)numCells;
	GLuint index = 0;


	std::vector<Vector4> positions =
	{
		// front
		Vector4(0, 0.0, 512.0, 1.0f),
		Vector4(512.0, 0.0, 512.0, 1.0f),

		Vector4(512.0, 0.0, 512.0, 1.0f),
		Vector4(512.0, 512.0, 512.0, 1.0f),

		Vector4(512.0f, 512.0f, 512.0f, 1.0f),
		Vector4(0.0f, 512.0, 512.0, 1.0f),

		Vector4(0.0f, 512.0f, 512.0f, 1.0f),
		Vector4(0.0f, 0.0f, 512.0f, 1.0f),

		// sides
		Vector4(0.0f, 0.0f, 512.0f, 1.0f),
		Vector4(0.0f, 0.0f, 0.0f, 1.0f),

		Vector4(512.0, 0.0, 512.0f, 1.0f),
		Vector4(512.0, 0.0, 0.0f, 1.0f),

		Vector4(512.0, 512.0, 512.0f, 1.0f),
		Vector4(512.0, 512.0, 0.0f, 1.0f),
			
		Vector4(0.0, 512.0, 512.0f, 1.0f),
		Vector4(0.0, 512.0, 0.0f, 1.0f),

		// back
		Vector4(0, 0.0, 0.0, 1.0f),
		Vector4(512.0, 0.0, 0.0, 1.0f),

		Vector4(512.0, 0.0, 0.0, 1.0f),
		Vector4(512.0, 512.0, 0.0, 1.0f),

		Vector4(512.0, 512.0, 0.0, 1.0f),
		Vector4(0.0, 512.0, 0.0, 1.0f),

		Vector4(0.0, 512.0, 0.0, 1.0f),
		Vector4(0.0, 0.0, 0.0, 1.0f)
	};

	std::vector<Vector4> positionsSmall
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
	void render(Matrix4 mvp, float dt);

	Matrix4 model = Matrix4::Translation(Vector3(0.0, 0.0, 0.0));



	GLuint counter;
	GLuint posVBO;
	GLuint vao;
	GLuint renderProgram;
	GLuint mvpLoc;
};
