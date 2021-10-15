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

void OpenGLView::setCubes(const std::vector<Cube*>* cubes)
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

    // Load texture source image
    int imgWidth, imgHeight, imgChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(CUBE_TEXTURE_PATH, &imgWidth, &imgHeight, &imgChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
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

double rho = 10;
float theta = 0;
float phi = 30;
float xFocal = 0;
float yFocal = -2.5;
float zFocal = -6.8;

void OpenGLView::paintGL()
{
    // Reset the background color and depth buffer
    glClearColor(0.67f, 0.95f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind cube texture to texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);

    // Set the active shader program for the current context
    shaderProgram->useProgram();

    // Comput the view and projection matrices
    float frustumAngle = 45; // Perspective angle of frustum
    glm::mat4 projection = glm::mat4(1.0f); // The projection matrix maps the cameras frame to clipping space


    float xPos = rho * sin(glm::radians(theta)) * cos(glm::radians(phi)) + xFocal;
    float yPos = rho * sin(glm::radians(phi)) + yFocal;
    float zPos = rho * cos(glm::radians(theta)) * cos(glm::radians(phi)) + zFocal;

    // The view matrix transforms the world frame to the cameras frame
    glm::mat4 view = glm::lookAt(glm::vec3(xPos, yPos, zPos),
        glm::vec3(xFocal, yFocal, zFocal),
        glm::vec3(0.0f, 1.0f, 0.0f));

    //view = glm::translate(view, glm::vec3(xPos, yPos, zPos -3.0f));
    //view = glm::rotate(view, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    //view = glm::rotate(view, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(frustumAngle), (float) screen_width / (float) screen_height, 0.1f, 100.0f);

    // Update the shader program's view and projection matrices
    shaderProgram->setUniformMat4("view", view);
    shaderProgram->setUniformMat4("projection", projection);

    // Render cubes
    if (cubes)
    {
        glBindVertexArray(vertArrayObj); // Bind to vertex array object containing the cube defintion
        for (unsigned int i = 0; i < cubes->size(); i++)
        {
            // Get cube to render
            Cube* cube = cubes->at(i);

            // Compute model matrix
            glm::mat4 model = glm::mat4(1.0f); // The model matrix is used to transform the local from to the world frame
            model = glm::translate(model, cube->getPosition());
            model = glm::rotate(model, cube->getYaw(), glm::vec3(1.0f, 1.0f, 0.0f));

            // Update the shader program's model matrix
            shaderProgram->setUniformMat4("model", model);

            // Draw the cube
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}


int mouseX = 0;
int mouseY = 0;
int thetaSensitivity = 30;
int phiSensitivity = 30;
int xFocalSensitivity = 5;
int zFocalSensitivity = 5;

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
        double deltaX = event->x() - mouseX;
        double deltaY = event->y() - mouseY;

        xFocal -= (deltaX * cos(glm::radians(theta)) + deltaY * sin(glm::radians(theta))) * ((xFocalSensitivity + 0.5 * rho) / 1000.0);
        zFocal -= (deltaY * cos(glm::radians(theta)) - deltaX * sin(glm::radians(theta))) * ((zFocalSensitivity + 0.5 * rho)  / 1000.0);

    }

    mouseX = event->x();
    mouseY = event->y();
}

void OpenGLView::mouseReleaseEvent(QMouseEvent* event)
{

}

void OpenGLView::keyPressEvent(QKeyEvent* event)
{

    
}

void OpenGLView::keyReleaseEvent(QKeyEvent* event)
{

}

void OpenGLView::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0) // up Wheel
        rho -= 5;
    else if (event->angleDelta().y() < 0) //down Wheel
        rho += 5;

    if (rho <= 0)
        rho = 5;
}