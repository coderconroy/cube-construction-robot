#pragma once

#include <QObject>
#include <QOpenGLFunctions_3_3_Core>
#include <sstream>
#include <glm/glm.hpp>

class ShaderProgram : public QObject, protected QOpenGLFunctions_3_3_Core
{
	Q_OBJECT
public:
	/*!
	* Class constructor. Reads the vertex and fragment source code from the specified files, compiles these shaders
	* and links them to the shader program represented by this class.
	* \param [in] vertexShaderPath File path to the vertex shader file.
	* \param [in] fragShaderPath File path to the fragment shader file.
	* \param [out] context Current OpenGL context.
	* \param [in] parent Parent object responsible for this \class ShaderProgram instance.
	*/
	ShaderProgram(const std::string& vertexShaderPath, const std::string& fragShaderPath, QOpenGLContext* context, QObject* parent = nullptr);
	
	/*!
	* Bind the current OpenGL context to the shader program.
	*/
	void useProgram();

	/*!
	* Set the integer value for the uniform in the shader program.
	* \param [in] uniformName Identifier of uniform to be set.
	* \param [in] value New value of uniform.
	*/
	void setUniformInt(const std::string& uniformName, int value) const;

	/*!
	* Set the float value for the uniform in the shader program.
	* \param [in] uniformName Identifier of uniform to be set.
	* \param [in] value New value of uniform.
	*/
	void setUniformFloat(const std::string& uniformName, float value) const;

	/*!
	* Set the float value for the uniform in the shader program.
	* \param [in] uniformName Identifier of uniform to be set.
	* \param [in] mat New value of uniform.
	*/
	void setUniformMat4(const std::string& uniformName, const glm::mat4& mat) const;

signals:
	/*!
	* Signal generated on the occurence of a shader program error.
	* \param [in] description Details of the error.
	*/
	void errorLog(const std::string& description);

private:
	QOpenGLContext* context; /*! OpenGL context in which the shader program is used */
	unsigned int shaderProgramID; /*! ID of the shader program encapsulated by this class instance */

	/*!
	* Read the contents of a file into a string.
	* \param [in] filePath Path of the source file.
	* \return String contents of the file.
	*/
	std::string readFile(const std::string& filePath);
};

