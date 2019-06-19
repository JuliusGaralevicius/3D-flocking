/*
Class: Boids
Author: Julius Garalevicius
Description: Simple shader class for compilation and error checking of either rendering programs or compute shader programs.
*/
#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <GL/glew.h>
class Shader
{

private:
	std::string RELATIVE_PATH = "../Shaders/";
	unsigned int ID;
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const GLchar* computePath);
	unsigned int getID() const { return ID; };

	~Shader();
};