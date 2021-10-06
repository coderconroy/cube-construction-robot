#include "OpenGLView.h"

void OpenGLView::initializeGL()
{
    // Set up the rendering context, load shaders and other resources, etc.:
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1.0f, 1.0f, 0.2f, 1.0f);
    //...
}

void OpenGLView::resizeGL(int w, int h)
{
    // Update projection matrix and other size related settings:
    //...
}

void OpenGLView::paintGL()
{
    // Draw the scene:
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT);

}