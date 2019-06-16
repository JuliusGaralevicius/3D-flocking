#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
class Shader
{
public:
	unsigned int ID;
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath);
	Shader(const GLchar* computePath);
	unsigned int getID() { return ID; };

	~Shader();
};