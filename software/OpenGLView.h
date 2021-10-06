#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class OpenGLView: public QOpenGLWidget
{
public:
    OpenGLView(QWidget* parent) : QOpenGLWidget(parent) {};
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};

