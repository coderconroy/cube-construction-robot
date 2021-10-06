#pragma once

#include "OpenGLView.h"
#include <QWidget>
#include <QVBoxLayout>

class DesignView: public QWidget
{
    Q_OBJECT
public:
    DesignView(QWidget* parent = Q_NULLPTR);
private:
    QVBoxLayout* baseLayout;
    OpenGLView* shapeView;
};