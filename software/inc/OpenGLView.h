#pragma once

#include "ShaderProgram.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>

class OpenGLView: public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    OpenGLView(QWidget* parent);
protected:
    /*!
    * Initialize OpenGL context.
    */
    void initializeGL() override;

    /*!
    * Update OpenGL viewport on window resize event.
    * \param [in] width - Updated \class OpenGLView widget width
    * \param [in] heihgt - Updated \class OpenGLView widget height
    */
    void resizeGL(int width, int height) override;

    /*!
    * Render the scene in the OpenGL viewport based on the current context.
    */
    void paintGL() override;

    /*!
    * Insert cube into the scene at the given position
    */
    void insertCube(unsigned int id, glm::vec3 position);
private:
    ShaderProgram* shaderProgram; /*! Shader program from vertex and fragment shaders */
    unsigned int vertArrayObj; /*! Vertex array buffer object ID */
    unsigned int cubeTexture; /*! Texture applied to each cube face */
    std::vector<glm::vec3> cubePositions; /*! Cube positions in the world reference frame */

    // Viewport parameters
    const unsigned int SCREEN_WIDTH = 800; /*! OpenGL viewport width */
    const unsigned int SCREEN_HEIGHT = 600; /*! OpenGL viewport height */

    // Shader paths
    const std::string VERT_SHADER_PATH = "src/shaders/shader.vert";
    const std::string FRAG_SHADER_PATH = "src/shaders/shader.frag";

    // Texture resource paths
    const char* CUBE_TEXTURE_PATH = "container.jpg";
};

