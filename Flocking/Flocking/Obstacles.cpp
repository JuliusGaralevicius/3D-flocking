#include "Obstacles.h"
#include "Shader.h"
#include <time.h>

Obstacles::Obstacles()
{
	Shader *rShader = new Shader("ObstacleVertex.glsl", "ObstacleFragment.glsl");
	renderProgram = rShader->getID();
	Shader *cShader = new Shader("ObstacleCompute.glsl");
	computeProgram = cShader->getID();

	mvpLoc = glGetUniformLocation(renderProgram, "mvp");
	mLoc = glGetUniformLocation(renderProgram, "m");
	vpLoc = glGetUniformLocation(renderProgram, "vp");

	deltaT = glGetUniformLocation(computeProgram, "dt");

	obstacle_data_t* obstacleData = new obstacle_data_t[OBSTACLE_COUNT];
	// initiate obstacles
	srand(time(0));
	for (int i = 0; i < OBSTACLE_COUNT; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			obstacleData[i].position[j] = (static_cast<float>(rand()) / static_cast <float> (RAND_MAX)) * 512;
			obstacleData[i].velocity[j] = ((static_cast<float>(rand()) / static_cast <float> (RAND_MAX)) -0.5)*0.5;
		}

		obstacleData[i].radius = (static_cast<float>(rand()) / static_cast <float> (RAND_MAX)) * (RADIUS_LIMITS[1] - RADIUS_LIMITS[0]) + RADIUS_LIMITS[0];
	}

	glGenBuffers(1, &obstacleSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, obstacleSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(obstacle_data_t)*OBSTACLE_COUNT, obstacleData, GL_DYNAMIC_COPY);
	
	GLuint posBuff;
	glGenBuffers(1, &posBuff);
	glBindBuffer(GL_ARRAY_BUFFER, posBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4)*positions.size(), positions.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &renderVAO);

	glBindVertexArray(renderVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, posBuff);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, obstacleSSBO);
	
	// bind position to 1
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, sizeof(obstacle_data_t), 0);
	// bind radius to 2
	glVertexAttribPointer((GLuint)2, 1, GL_FLOAT, GL_FALSE, sizeof(obstacle_data_t), (void*)(sizeof(Vector3)));

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

}

void Obstacles::Render(double dt, Matrix4 vp, float time)
{
	glUseProgram(computeProgram);
	glUniform1f(deltaT, dt);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, obstacleSSBO);
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_COMPUTE_SHADER_BIT);

	glUseProgram(renderProgram);


	glBindVertexArray(renderVAO);
	Matrix4 mvp = vp * model;
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.values);
	glUniformMatrix4fv(vpLoc, 1, GL_FALSE, vp.values);
	glUniformMatrix4fv(mLoc, 1, GL_FALSE, model.values);

	glDrawArraysInstanced(GL_TRIANGLES, 0, positions.size(), OBSTACLE_COUNT);
	glBindVertexArray(0);
}


Obstacles::~Obstacles()
{

	
}
