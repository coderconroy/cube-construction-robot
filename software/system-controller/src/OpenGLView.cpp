#include "OpenGLView.h"
#include "iostream"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb-image/stb_image.h>

OpenGLView::OpenGLView(QWidget* parent) : QOpenGLWidget(parent) 
{
    // Set focus policy to ensure the widget receives keyboard events
    setFocusPolicy(Qt::ClickFocus);
}

void OpenGLView::setCubes(const QList<Cube*>* cubes)
{
    this->cubes = cubes;
}

void OpenGLView::initializeGL()
{
    // Initialize the OpenGL function pointers
    initializeOpenGLFunctions();

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Initialize shaders and shader program
    shaderProgram = new ShaderProgram(VERT_SHADER_PATH, FRAG_SHADER_PATH, context(), this);

    // Initialize cube vertex data
    // Each line corresponds to a triangle vertex and the attributes for each vertex are defined as
    // [x position, y position, z position, x texture coordinate, y texture coordinate]
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // Initialize vertex array and vertex buffer objects
    unsigned int vertBufferObj;
    glGenBuffers(1, &vertBufferObj);
    glGenVertexArrays(1, &vertArrayObj);

    // Bind vertex array object and vertex buffer object to current context
    glBindVertexArray(vertArrayObj);
    glBindBuffer(GL_ARRAY_BUFFER, vertBufferObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Initialize vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Texture coordinate attribute
    glEnableVertexAttribArray(1);

    // Initialize cube texture
    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_2D, cubeTexture); // Bind texture to current context
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Texture mapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load cube texture source image
    int imgWidth, imgHeight, imgChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(CUBE_TEXTURE_PATH, &imgWidth, &imgHeight, &imgChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Initialize selected cube texture
    glGenTextures(1, &cubeTextureSelected);
    glBindTexture(GL_TEXTURE_2D, cubeTextureSelected); // Bind texture to current context
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Texture mapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load selected cube texture source image
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(CUBE_TEXTURE_SELECTED_PATH, &imgWidth, &imgHeight, &imgChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Initialize invalid cube texture
    glGenTextures(1, &cubeTextureInvalid);
    glBindTexture(GL_TEXTURE_2D, cubeTextureInvalid); // Bind texture to current context
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Texture mapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load selected cube texture source image
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(CUBE_TEXTURE_INVALID_PATH, &imgWidth, &imgHeight, &imgChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Initialize the shader program's texture sampler's
    shaderProgram->useProgram();
    shaderProgram->setUniformInt("cubeTexture", 0);
}

void OpenGLView::resizeGL(int width, int height)
{
    // Update OpenGL viewport size
    // TODO: Identify if any code is required here
    // glViewport(0, 0, width, height);
    screen_width = width;
    screen_height = height;
}

void OpenGLView::paintGL()
{
    // Reset the background color and depth buffer
    glClearColor(0.67f, 0.95f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the active shader program for the current context
    shaderProgram->useProgram();

    // Compute the view and projection matrices
    float frustumAngle = 45; // Perspective angle of frustum
    glm::mat4 projection = glm::mat4(1.0f); // The projection matrix maps the cameras frame to clipping space

    float xPos = rho * sin(glm::radians(theta)) * cos(glm::radians(phi)) + xFocal;
    float yPos = rho * sin(glm::radians(phi)) + yFocal;
    float zPos = rho * cos(glm::radians(theta)) * cos(glm::radians(phi)) + zFocal;

    // The view matrix transforms the world frame to the cameras frame
    glm::mat4 view = createViewMatrix(glm::vec3(xPos, yPos, zPos), glm::vec3(xFocal, yFocal, zFocal), glm::vec3(0.0f, 1.0f, 0.0f));

    // The projection matrix transforms the camera frame to clip coordinates and computes the NDC from this
    projection = createProjectionMatrix(glm::radians(frustumAngle), (float) screen_width / (float) screen_height, 0.1f, 10000.0f);

    // Update the shader program's view and projection matrices
    shaderProgram->setUniformMat4("view", view);
    shaderProgram->setUniformMat4("projection", projection);

    // Render cubes
    if (cubes)
    {
        glBindVertexArray(vertArrayObj); // Bind to vertex array object containing the cube defintion
        for (unsigned int i = 0; i < cubes->size(); i++)
        {
            // Get next cube to render
            Cube* cube = cubes->at(i);

            // Bind cube texture to texture unit based on cube state
            glActiveTexture(GL_TEXTURE0);
            if (cube->getState() == CubeState::VALID)
                glBindTexture(GL_TEXTURE_2D, cubeTexture);
            else if (cube->getState() == CubeState::SELECTED)
                glBindTexture(GL_TEXTURE_2D, cubeTextureSelected);
            else if (cube->getState() == CubeState::INVALID)
                glBindTexture(GL_TEXTURE_2D, cubeTextureInvalid);

            // Compute model matrix
            glm::mat4 model = glm::mat4(1.0f); // The model matrix is used to transform the local frame to the world frame
            model = translateMatrix(model, cube->getPosition());
            model = rotateMatrixY(model, cube->getPitch());
            // Scale the cube from one step in size to its side length in steps
            model = scaleMatrix(model, glm::vec3(cube->getSideLength(), cube->getSideLength(), cube->getSideLength()));

            // Update the shader program's model matrix
            shaderProgram->setUniformMat4("model", model);

            // Draw the cube
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

void OpenGLView::mousePressEvent(QMouseEvent* event)
{
    mouseX = event->x();
    mouseY = event->y();
}

void OpenGLView::mouseMoveEvent(QMouseEvent* event)
{

    if (event->buttons() == Qt::RightButton)
    {
        int deltaX = event->x() - mouseX;
        int deltaY = event->y() - mouseY;

        theta -= deltaX * (thetaSensitivity / 100.0);
        phi += deltaY * (phiSensitivity / 100.0);

        if (phi > 89.9)
            phi = 89.9;
        else if (phi < 0)
            phi = 0;
    }
    if (event->buttons() == Qt::MiddleButton)
    {
        int deltaX = event->x() - mouseX;
        int deltaY = event->y() - mouseY;

        xFocal -= ((double) deltaX * cos(glm::radians(theta)) + (double) deltaY * sin(glm::radians(theta))) * ((xFocalSensitivity + 0.5 * (double) rho) / 1000.0);
        zFocal -= ((double) deltaX * -sin(glm::radians(theta)) + (double) deltaY * cos(glm::radians(theta))) * ((zFocalSensitivity + 0.5 * (double) rho)  / 1000.0);

    }

    mouseX = event->x();
    mouseY = event->y();
}

void OpenGLView::mouseReleaseEvent(QMouseEvent* event)
{

}

bool shiftKeyPressed = false;

void OpenGLView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Up)
    {
        if(shiftKeyPressed)
            emit cubePositionUpdateRequested(0, 0, 1); // Increment cube layer
        else
            emit cubePositionUpdateRequested(0, -1, 0); // Move cube back
    }
    else if (event->key() == Qt::Key_Down)
    {
        if (shiftKeyPressed)
            emit cubePositionUpdateRequested(0, 0, -1); // Decrement cube layer
        else
            emit cubePositionUpdateRequested(0, 1, 0); // Move cube forward
    }
    else if (event->key() == Qt::Key_Left)
    {
        if (shiftKeyPressed)
            emit cubeOrientationUpdateRequested(1); // Rotate cube
        else
            emit cubePositionUpdateRequested(-1, 0, 0); // Move cube left
    }
    else if (event->key() == Qt::Key_Right)
    {
        if (shiftKeyPressed)
            emit cubeOrientationUpdateRequested(-1); // Rotate cube
        else
            emit cubePositionUpdateRequested(1, 0, 0); // Move cube right
    }
    else if (event->key() == Qt::Key_Shift)
        shiftKeyPressed = true;
}

void OpenGLView::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift)
        shiftKeyPressed = false;
}

void OpenGLView::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0) // up Wheel
        rho -= 100;
    else if (event->angleDelta().y() < 0) //down Wheel
        rho += 100;

    if (rho <= 0)
        rho = 100;
}

glm::mat4 OpenGLView::translateMatrix(glm::mat4 const& mat, glm::vec3 const& vec) const
{
    // Initialise output matrix
    glm::mat4 output = glm::mat4(1.0f);

    // Insert translations into matrix
    output[3][0] = vec[0];
    output[3][1] = vec[1];
    output[3][2] = vec[2];

    // Post-multiply with input matrix
    return mat * output;
}

glm::mat4 OpenGLView::rotateMatrixY(const glm::mat4& mat, const float theta) const
{
    // Initialise y axis rotation matrix
    glm::mat4 yRot = glm::mat4(1.0f);

    yRot[0][0] = cos(theta);
    yRot[2][0] = sin(theta);
    yRot[0][2] = -sin(theta);
    yRot[2][2] = cos(theta);

    // Post-multiply with input matrix
    return mat * yRot;
}

glm::mat4 OpenGLView::scaleMatrix(const glm::mat4& mat, const glm::vec3& vec) const
{
    // Initialise scale matrix
    glm::mat4 scale = glm::mat4(1.0f);

    scale[0][0] = vec[0];
    scale[1][1] = vec[1];
    scale[2][2] = vec[2];

    // Post-multiply with input matrix
    return mat * scale;
}

glm::mat4 OpenGLView::createViewMatrix(const glm::vec3& cameraPos, const glm::vec3& focalPoint, const glm::vec3& upVector) const
{
    // Compute view space coordinate axes
    const glm::vec3 zDirection(glm::normalize(cameraPos - focalPoint));
    const glm::vec3 xDirection(glm::normalize(glm::cross(upVector, zDirection)));
    const glm::vec3 yDirection(glm::cross(zDirection, xDirection));

    // Compute left matrix
    glm::mat4 output1(1.0f);
    output1[0][0] = xDirection.x;
    output1[1][0] = xDirection.y;
    output1[2][0] = xDirection.z;
    output1[0][1] = yDirection.x;
    output1[1][1] = yDirection.y;
    output1[2][1] = yDirection.z;
    output1[0][2] = zDirection.x;
    output1[1][2] = zDirection.y;
    output1[2][2] = zDirection.z;

    // Compute right matrix
    glm::mat4 output2(1.0f);
    output2[3][0] = -cameraPos.x;
    output2[3][1] = -cameraPos.y;
    output2[3][2] = -cameraPos.z;

    // Compute view matrix
    return output1 * output2;
}

glm::mat4 OpenGLView::createProjectionMatrix(const float fovy, const float aspect, const float zPlaneClose, const float zPlaneFar) const
{
    const float tanHalfFovy = tan(fovy * 0.5f);

    glm::mat4 output(0.0f);
    output[0][0] = 1.0f / (aspect * tanHalfFovy);
    output[1][1] = 1.0f / (tanHalfFovy);
    output[2][2] = -(zPlaneFar + zPlaneClose) / (zPlaneFar - zPlaneClose);
    output[2][3] = -1.0f;
    output[3][2] = -(2.0f * zPlaneFar * zPlaneClose) / (zPlaneFar - zPlaneClose);
    return output;
}