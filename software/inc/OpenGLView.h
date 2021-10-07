#pragma once

#include "ShaderProgram.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

class OpenGLView: public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    OpenGLView(QWidget* parent) : QOpenGLWidget(parent) {};
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
private:
    ShaderProgram* shaderProgram; /*! Shader program from vertex and fragment shaders */
    unsigned int VAO; /*! Vertex array buffer object ID */
};

