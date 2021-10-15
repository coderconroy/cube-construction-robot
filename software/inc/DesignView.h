#pragma once

#include "OpenGLView.h"
#include "CubeWorldModel.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>

class DesignView: public QWidget
{
    Q_OBJECT
public:
    DesignView(QWidget* parent = Q_NULLPTR);

signals:
    /*!
    * Generated when a message is logged by an \class OpenGLView instance.
    */
    void log(Message message);

private:
    QVBoxLayout* baseLayout;
    OpenGLView* shapeView;
    CubeWorldModel* cubeWorldModel;
    QTimer* openGLTimer; /*! Timer to trigger update of OpenGL shape view */

    /*!
    * Request OpenGL redraw the shape view.
    */
    void updateShapeView();
};