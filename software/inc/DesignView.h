#pragma once

#include "OpenGLView.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>

class DesignView: public QWidget
{
    Q_OBJECT
public:
    DesignView(QWidget* parent = Q_NULLPTR);
private:
    QVBoxLayout* baseLayout;
    OpenGLView* shapeView;
    QTimer* openGLTimer; /*! Timer to trigger update of OpenGL shape view */

    /*!
    * Request OpenGL redraw the shape view.
    */
    void updateShapeView();
};