#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

class OpenGLView: public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
public:
    OpenGLView(QWidget* parent) : QOpenGLWidget(parent) {};
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};

