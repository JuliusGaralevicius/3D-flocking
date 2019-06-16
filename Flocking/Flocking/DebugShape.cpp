#include "DebugShape.h"
#include "Shader.h"


DebugShape::DebugShape(bool big, GLuint boidCounts)
{
	this->counter = boidCounts;
	Shader *s = new Shader("VertShaderDEBUG.glsl", "FragShaderDEBUG.glsl", "");
	renderProgram = s->getID();

	// positions for single cube
	// pass the cube id
	// offset the cube based on id

	mvpLoc = glGetUniformLocation(renderProgram, "mvp");

	// setup geometry VBO
	GLuint posBuf;
	glGenBuffers(1, &posBuf);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf);
	if (big)
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4)*positions.size(), positions.data(), GL_STATIC_DRAW);
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4)*positionsSmall.size(), positionsSmall.data(), GL_STATIC_DRAW);
	}
	// setup geometry VBO

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, posBuf);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(0);

	glBindVertexArray(0);

}


DebugShape::~DebugShape()
{

}

void DebugShape::render(Matrix4 vp, float dt)
{
	glUseProgram(renderProgram);
	glBindVertexArray(vao);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, counter);
	Matrix4 mvp = vp*model;
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.values);
	glDrawArraysInstanced(GL_LINES, 0, positionsSmall.size(), 16 * 16 * 16);
	glBindVertexArray(0);
}
