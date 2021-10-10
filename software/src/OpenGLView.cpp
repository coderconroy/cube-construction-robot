#include "OpenGLView.h"
#include "iostream"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb-image/stb_image.h>

float angle = 0;

OpenGLView::OpenGLView(QWidget* parent) : QOpenGLWidget(parent) {}

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
    glm::mat4 view = glm::mat4(1.0f); // The view matrix transforms the world frame to the cameras frame
    glm::mat4 projection = glm::mat4(1.0f); // The projection matrix maps the cameras frame to clipping space

    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(frustumAngle), (float) screen_width / (float) screen_height, 0.1f, 100.0f);

    // Update the shader program's view and projection matrices
    shaderProgram->setUniformMat4("view", view);
    shaderProgram->setUniformMat4("projection", projection);

    // Render cubes
    glBindVertexArray(vertArrayObj); // Bind to vertex array object containing the cube defintion
    for (unsigned int i = 0; i < cubePositions.size(); i++)
    {
        // Compute model matrix
        glm::mat4 model = glm::mat4(1.0f); // The model matrix is used to transform the local from to the world frame
        model = glm::translate(model, cubePositions[i]);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 0.0f));

        // Update the shader program's model matrix
        shaderProgram->setUniformMat4("model", model);

        // Draw the cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void OpenGLView::mousePressEvent(QMouseEvent* event)
{
    angle += 5;
}

void OpenGLView::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() == Qt::RightButton)
        angle += 5;
}

void OpenGLView::mouseReleaseEvent(QMouseEvent* event)
{
    angle += 5;
}

void OpenGLView::insertCube(glm::vec3 position)
{
    cubePositions.push_back(position);
}