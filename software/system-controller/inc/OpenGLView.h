#pragma once

#include "ShaderProgram.h"
#include "Cube.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QList>
#include <vector>
#include "Logger.h"

class OpenGLView: public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    /*!
    * Class constructor. The class OpenGL initialization is not performed here.
    */
    OpenGLView(QWidget* parent = Q_NULLPTR);
    
    /*!
    * Setter for list of cubes in the world reference frame that need to be rendered.
    * \param cubes Cubes to be rendered.
    */
    void setCubes(const QList<Cube*>* cubes);

signals:
    /*!
    * Signal generated when a message is logged by an \class OpenGLView instance.
    */
    void log(Message message) const;

    /*!
    * Signal generated when an input to update the position of the selected cube is received.
    */
    void cubePositionUpdateRequested(const int deltaX, const int deltaZ, const int deltaLayer);

    /*!
    * Signal generated when an input to update the orientation of the selected cube is received.
    */
    void cubeOrientationUpdateRequested(const int angleSteps);

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
    * Event handler for when the mouse is pressed.
    * \param [in] Mouse event description.
    */
    void mousePressEvent(QMouseEvent* event) override;

    /*!
    * Event handler for when the mouse is moved.
    * \param [in] Mouse event description.
    */
    void mouseMoveEvent(QMouseEvent* event) override;

    /*!
    * Event handler for when the mouse is released.
    * \param [in] Mouse event description.
    */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /*!
    * Event handler for when the key is pressed.
    * \param [in] Key event description.
    */
    void keyPressEvent(QKeyEvent* event) override;

    /*!
     * Event handler for when the key is released.
     * \param [in] Key event description.
     */
    void keyReleaseEvent(QKeyEvent* event) override;

    /*!
     * Event handler for when the scroll wheel is rotated;
     * \param [in] Scroll wheel event description.
     */
    void wheelEvent(QWheelEvent* event);

private:
    ShaderProgram* shaderProgram; /*! Shader program from vertex and fragment shaders */
    unsigned int vertArrayObj; /*! Vertex array buffer object ID */
    unsigned int cubeTexture; /*! Texture applied to each valid cube face */
    unsigned int cubeTextureSelected; /*! Texture applied to each selected cube face */
    unsigned int cubeTextureInvalid; /*! Texture applied to each invalid cube face */
    const QList<Cube*>* cubes = Q_NULLPTR; /*! Cubes in the world reference frame to be rendered */

    // Viewport parameters
    unsigned int screen_width; /*! OpenGL viewport width */
    unsigned int screen_height; /*! OpenGL viewport height */

    // Shader paths
    const std::string VERT_SHADER_PATH = "src/shaders/shader.vert";
    const std::string FRAG_SHADER_PATH = "src/shaders/shader.frag";

    // Texture resource paths
    const char* CUBE_TEXTURE_PATH = "res/cube-texture.jpg";
    const char* CUBE_TEXTURE_SELECTED_PATH = "res/cube-texture-selected.jpg";
    const char* CUBE_TEXTURE_INVALID_PATH = "res/cube-texture-invalid.jpg";

    // Camera control parameters
    int rho = 800;
    float theta = 0;
    float phi = 30;
    float xFocal = 0;
    float yFocal = 0;
    float zFocal = 0;
    int mouseX = 0;
    int mouseY = 0;
    int thetaSensitivity = 30;
    int phiSensitivity = 30;
    int xFocalSensitivity = 5;
    int zFocalSensitivity = 5;

    /*!
    * Post-multiply 4x4 homogeneous matrix by 4x4 translation matrix.
    * 
    * \param [in] mat The matrix to be updated.
    * \param [in] vec Vector containing the translation values for the x, y and z axes.
    */
    glm::mat4 translateMatrix(glm::mat4 const& mat, glm::vec3 const& vec) const;

    /*!
    * Post-multiply 4x4 homogeneous matrix by 4x4 rotation matrix for rotation about the y-axis
    *
    * \param [in] mat The matrix to be updated.
    * \param [in] theta Angle of rotation about y-axis (radians).
    */
    glm::mat4 rotateMatrixY(const glm::mat4& mat, const float theta) const;

    /*!
    * Post-multiply 4x4 homogeneous matrix by 4x4 scaling matrix.
    *
    * \param [in] mat The matrix to be updated.
    * \param [in] vec Vector containing the scaling factors for the x, y and z axes.
    */
    glm::mat4 scaleMatrix(const glm::mat4& mat, const glm::vec3& vec) const;

    /*!
    * Create view matrix for mapping the world space to view space.
    *
    * \param [in] cameraPos Position of the camera in the world frame.
    * \param [in] focalPoint Position of the focal point of the camera in the world frame.
    * \param [in] upVector Direction vector for the up direction in the world frame.
    */
    glm::mat4 createViewMatrix(const glm::vec3& cameraPos, const glm::vec3& focalPoint, const glm::vec3& upVector) const;

    glm::mat4 createProjectionMatrix(const float fovy, const float aspect, const float zNear, const float zFar) const;
};