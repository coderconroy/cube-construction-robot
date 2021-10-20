#include "ShaderProgram.h"
#include <fstream>
#include <QOpenGLVersionFunctionsFactory>


ShaderProgram::ShaderProgram(const std::string& vertShaderPath, const std::string& fragShaderPath, QOpenGLContext* context, QObject* parent)
	: QObject(parent)
{
	// Initialize member variables
	this->context = context;

	// Get object that provides access to OpenGL functions for the given context and profile
	auto f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context);

	// Read shader code from files
	std::string vertShaderCodeStr = readFile(vertShaderPath);
	std::string fragShaderCodeStr = readFile(fragShaderPath);
	const char* vertShaderCode = vertShaderCodeStr.c_str();
	const char* fragShaderCode = fragShaderCodeStr.c_str();

	// Initialize vertex shader
	unsigned int vertexShaderID = f->glCreateShader(GL_VERTEX_SHADER);
	f->glShaderSource(vertexShaderID, 1, &vertShaderCode, NULL);
	f->glCompileShader(vertexShaderID);

	// Initialize fragment shader
	unsigned int fragmentShaderID = f->glCreateShader(GL_FRAGMENT_SHADER);
	f->glShaderSource(fragmentShaderID, 1, &fragShaderCode, NULL);
	f->glCompileShader(fragmentShaderID);

	// Link vertex and fragment shader to shader program
	shaderProgramID = f->glCreateProgram();
	f->glAttachShader(shaderProgramID, vertexShaderID);
	f->glAttachShader(shaderProgramID, fragmentShaderID);
	f->glLinkProgram(shaderProgramID);

	// Free vertex and fragment shaders used to create shader program
	f->glDeleteShader(vertexShaderID);
	f->glDeleteShader(fragmentShaderID);
}

void ShaderProgram::useProgram()
{
	// Get object that provides access to OpenGL functions for the given context and profile
	auto f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context);

	// Bind shader program to context
	f->glUseProgram(shaderProgramID);
}

void ShaderProgram::setUniformInt(const std::string& uniformName, int value) const
{
	auto f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context);
	f->glUniform1i(f->glGetUniformLocation(shaderProgramID, uniformName.c_str()), value);
}

void ShaderProgram::setUniformFloat(const std::string& uniformName, float value) const
{
	auto f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context);
	f->glUniform1f(f->glGetUniformLocation(shaderProgramID, uniformName.c_str()), value);
}

void ShaderProgram::setUniformMat4(const std::string& uniformName, const glm::mat4& mat) const
{
	auto f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(context);
	f->glUniformMatrix4fv(f->glGetUniformLocation(shaderProgramID, uniformName.c_str()), 1, GL_FALSE, &mat[0][0]);
}

std::string ShaderProgram::readFile(const std::string& filePath)
{
	std::string content;
	std::ifstream inputFile;

	// Read content from file specified by file path
	inputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		inputFile.open(filePath); // Open file
		std::stringstream fileStream;
		fileStream << inputFile.rdbuf(); // Read file buffer into stream
		content = fileStream.str(); // Convert stream to string
	}
	catch (std::ifstream::failure& e)
	{
		emit errorLog("Shader Program: Failed to read file '" + filePath + "'");
	}

	return content;
}