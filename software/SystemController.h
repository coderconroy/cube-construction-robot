#pragma once

#include <QtWidgets/QWidget>
#include "ui_SystemController.h"

class SystemController : public QWidget
{
    Q_OBJECT

public:
    SystemController(QWidget *parent = Q_NULLPTR);

private:
    Ui::SystemControllerClass ui;
};
