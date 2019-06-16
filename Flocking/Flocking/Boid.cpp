#include "Boid.h"
#include <time.h>
#include "Shader.h"
#include <GLFW/glfw3.h>
#include "Obstacles.h"
void Boid::toggleAvoidance()
{
	avoid = avoid > 0.5f ? 0.0f : 1.0f;
}
void Boid::setParams(float avoidance, float homing)
{
	this->avoidance = avoidance;
	this->homingValue = homing;
}
Boid::Boid(Obstacles* obs, int size, Vector4 colour1, Vector4 colour2) : FLOCKSIZE(size)
{
	this->colour1 = colour1;
	this->colour2 = colour2;
	this->obs = obs;
	Shader *cShader = new Shader("ComputeTest.glsl");
	Shader *rShader = new Shader("VertexShader.glsl", "FragShader.glsl", "");
	Shader *hashShader = new Shader("ComputeHash.glsl");
	Shader *scanShader = new Shader("ComputeScan.glsl");
	Shader *combine = new Shader("ComputeCombine.glsl");
	Shader *sortShader = new Shader("ComputeSort.glsl");



	computeScan = scanShader->getID();
	computeHash = hashShader->getID();
	IDShader = rShader->getID();
	computePositions = cShader->getID();
	computeSort = sortShader->getID();
	computeCombine = combine->getID();

	col1 = glGetUniformLocation(IDShader, "col1");
	col2 = glGetUniformLocation(IDShader, "col2");



	avoidObstaclesBool = glGetUniformLocation(computePositions, "avoidObstacles");
	goalLoc[0] = glGetUniformLocation(computePositions, "goal1");
	goalLoc[1] = glGetUniformLocation(computePositions, "goal2");
	dtLoc = glGetUniformLocation(computePositions, "dt");
	mvpLoc = glGetUniformLocation(IDShader, "mvp");

	avoidanceLoc = glGetUniformLocation(computePositions, "avoidRadius");
	homingLoc = glGetUniformLocation(computePositions, "homing");



	boid_data_t* boidData = new boid_data_t[FLOCKSIZE];
	// initiate positions
	srand(time(0));
	for (int i = 0; i < FLOCKSIZE/2; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			boidData[i].velocity[j] = 0;// (static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
			boidData[i].position[j] = (static_cast<float>(rand()) / static_cast <float> (RAND_MAX))* 512;
		}
		boidData[i].type = 1.0f;
		boidData[i].position[3] = 0;
	}
	for (int i = FLOCKSIZE/2; i < FLOCKSIZE; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			boidData[i].velocity[j] = 0;// (static_cast<float>(rand()) / static_cast <float> (RAND_MAX));
			boidData[i].position[j] = (static_cast<float>(rand()) / static_cast <float> (RAND_MAX)) * 512;
		}
		boidData[i].type = 0.0f;
		boidData[i].position[3] = 0;
	}
	// initiate positions

	// setup BOID SSBO
	glGenBuffers(2,  ssboBoids);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBoids[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(boid_data_t)*FLOCKSIZE, boidData, GL_DYNAMIC_COPY);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBoids[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(boid_data_t)*FLOCKSIZE, NULL, GL_DYNAMIC_COPY);

	// setup sum SSBO
	glGenBuffers(1, &sumSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sumSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint)*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION, NULL, GL_DYNAMIC_COPY);

	// setup original sum SSBO (store all beginning indexes for each cell, same as sum SSBO before sorting)
	glGenBuffers(1, &sumSSBOOG);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sumSSBOOG);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint)*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION, NULL, GL_DYNAMIC_COPY);

	// generate atomic counter
	glGenBuffers(1, &atomicCounter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * CELLCOUNTDIMENSION*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	// setup SSBO for storing max count of each local scan group
	glGenBuffers(1, &maxSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, maxSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint)*4, NULL, GL_DYNAMIC_COPY);

	// setup geometry VBO
	GLuint posBuff;
	glGenBuffers(1, &posBuff);
	glBindBuffer(GL_ARRAY_BUFFER, posBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4)*positions.size(), positions.data(), GL_STATIC_DRAW);
	// setup geometry VBO

	// setup VAO for rendering
	glGenVertexArrays(2, renderVAO);
	for (int i = 0; i < 2; i++)
	{
		glBindVertexArray(renderVAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, posBuff);
		glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, ssboBoids[i]);

		glVertexAttribPointer((GLuint)2, 4, GL_FLOAT, GL_FALSE, sizeof(boid_data_t), 0);
		glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, sizeof(boid_data_t), (void*)(sizeof(Vector4)));
		glVertexAttribPointer((GLuint)4, 1, GL_FLOAT, GL_FALSE, sizeof(boid_data_t), (void*)(sizeof(Vector3)+ sizeof(Vector4)));


		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
	}

	glBindVertexArray(0);

}
void Boid::render(double deltaTime, Vector4 goal1, Vector4 goal2, Matrix4 vp)
{
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint) * CELLCOUNTDIMENSION*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION, emptyArr);

	bool DEBUG = false;
	// Compute hash for each Boid
	glUseProgram(computeHash);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicCounter);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboBoids[odd]);
	
	glDispatchCompute(FLOCKSIZE / 256, 1, 1);
	glMemoryBarrier(GL_COMPUTE_SHADER_BIT);
	// Compute hash for each Boid

	if (DEBUG)
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomicCounter);
		GLuint *ptr;
		ptr = (GLuint*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		int sum = 0;
			for (int i = 1; i <= CELLCOUNTDIMENSION*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION; i++)
			{
				std::cout << sum << " ";
				sum += ptr[i - 1];
				if ((i + 1) % 30 == 0)
					std::cout << std::endl;
			}

		std::cout << std::endl << std::endl;
	}
	// Calculate prefix sum
	glUseProgram(computeScan);

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomicCounter);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sumSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, maxSSBO);

	// perform 4 local prefix scans
	glDispatchCompute(4, 1, 1);
	glMemoryBarrier(GL_COMPUTE_SHADER_BIT);

	if (DEBUG)
	{
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, sumSSBO);
		//GLuint *pt = (GLuint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		//for (int i = 0; i < CELLCOUNTDIMENSION*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION; i++)
		//{
		//	std::cout << " " << pt[i];
		//	if ((i + 1) % 30 == 0)
		//		std::cout << std::endl;
		//}	
		//
		//std::cout << std::endl << std::endl;
		//
		//
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, maxSSBO);
		//pt = (GLuint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		//
		//for (int i = 0; i < 4; i++)
		//{
		//	std::cout << pt[i] << "\n";
		//}
	}
	// combine the prefix scans
	glUseProgram(computeCombine);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, sumSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sumSSBOOG);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, maxSSBO);

	glDispatchCompute(FLOCKSIZE/256, 1, 1);
	glMemoryBarrier(GL_COMPUTE_SHADER_BIT);
	if (DEBUG)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, sumSSBO);
		GLuint *pt = (GLuint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		for (int i = 0; i < CELLCOUNTDIMENSION*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION; i++)
		{
			std::cout << " " << pt[i];
			if ((i + 1) % 30 == 0)
				std::cout << std::endl;
		}

		std::cout << std::endl << std::endl;
	}
	// sort boids
	glUseProgram(computeSort);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, sumSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboBoids[odd]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboBoids[!odd]);

	glDispatchCompute(FLOCKSIZE/256, 1, 1);

	glMemoryBarrier(GL_COMPUTE_SHADER_BIT);

	// sort boids
	if (DEBUG)
	{
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, sumSSBO);
		GLuint *pt = (GLuint*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		for (int i = 0; i < CELLCOUNTDIMENSION*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION; i++)
		{
			std::cout << " " << pt[i];
			if ((i + 1) % 30 == 0)
				std::cout << std::endl;
		}
		int n;

		std::cin >> n;
		std::cout << std::endl << "---------------------START OF OG BUFFER--------------" << std::endl;

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, sumSSBOOG);
		pt = (GLuint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		for (int i = 0; i < CELLCOUNTDIMENSION*CELLCOUNTDIMENSION*CELLCOUNTDIMENSION; i++)
		{
			std::cout << " " << pt[i];
			if ((i + 1) % 30 == 0)
				std::cout << std::endl;
		}
		std::cout << std::endl << "-----------------END OF OG BUFFER ----------------------" << std::endl;
		std::cin >> n;
	}

	// calculate new positions
	glUseProgram(computePositions);

	glUniform1f(avoidanceLoc, avoidance);
	glUniform1f(homingLoc, homingValue);
	glUniform1f(avoidObstaclesBool, avoid);

	glUniform1f(dtLoc, deltaTime);
	glUniform4f(goalLoc[0], goal1.x, goal1.y, goal1.z, goal1.w);
	glUniform4f(goalLoc[1], goal2.x, goal2.y, goal2.z, goal2.w);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboBoids[!odd]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sumSSBOOG);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, obs->obstacleSSBO);
	glDispatchCompute(FLOCKSIZE/256, 1, 1);
	glMemoryBarrier(GL_COMPUTE_SHADER_BIT);
	// render
	glUseProgram(IDShader);
	glBindVertexArray(renderVAO[!odd]);
	glUniform4f(col1, colour1.x, colour1.y, colour1.z, colour1.w);
	glUniform4f(col2, colour2.x, colour2.y, colour2.z, colour2.w);

	//std::cout << vp.GetPositionVector() << std::endl;
	Matrix4 mvp = vp*model;
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.values);
	glDrawArraysInstanced(GL_TRIANGLES, 0, positions.size(), FLOCKSIZE);
	glBindVertexArray(0);

	odd = !odd;
}

Boid::~Boid()
{

}
