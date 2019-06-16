#include "Shader.h"


Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath, const GLchar * geometryPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;


	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	//gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try 
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		//gShaderFile.open(geometryPath);

		std::stringstream vShaderStream, fShaderStream, gShaderStream;
		
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//gShaderStream << gShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();
		gShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		//geometryCode = gShaderStream.str();
	}
	catch (std::ifstream::failure e) 
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	//const char* gShaderCode = geometryCode.c_str();

	unsigned int vertex, fragment, geometry;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "Vertex shader compilation failed\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Fragment shader compilation failed\n" << infoLog << std::endl;
	}

	//geometry = glCreateShader(GL_GEOMETRY_SHADER);
	//glShaderSource(geometry, 1, &vShaderCode, NULL);
	//glCompileShader(geometry);
	//glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
	//if (!success)
	//{
	//	glGetShaderInfoLog(geometry, 512, NULL, infoLog);
	//	std::cout << "Geometry shader compilation failed\n" << infoLog << std::endl;
	//}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	//glAttachShader(ID, geometry);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	//glDeleteShader(geometry);
}

Shader::Shader(const GLchar * computePath)
{
	std::string computeCode;
	std::ifstream computeFile;

	computeFile.exceptions(std::ifstream::badbit || std::ifstream::failbit);
	try
	{
		computeFile.open(computePath);
		std::stringstream computeStream;
		computeStream << computeFile.rdbuf();

		computeFile.close();
		computeCode = computeStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Compute shader not read" << std::endl;
	}

	const char* cShaderCode = computeCode.c_str();
	unsigned int computeShader;

	computeShader = glCreateShader(GL_COMPUTE_SHADER);
	int success;
	char infolog[512] = { 'a' };
	glShaderSource(computeShader, 1, &cShaderCode, NULL);
	glCompileShader(computeShader);

	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(computeShader, 512, NULL, infolog);
		std::cout << "compute shader compilation failed\n" << infolog << std::endl;
	}
	else
	{
		unsigned int program = glCreateProgram();
		glAttachShader(program, computeShader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 512, NULL, infolog);
			std::cout << "Compute shader linking failed\n" << infolog << std::endl;
		}
		else
		{
			ID = program;
		}
	}
	glDeleteShader(computeShader);
}

Shader::~Shader()
{
}
