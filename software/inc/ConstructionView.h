#pragma once

#include "Robot.h"
#include "Logger.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>

class ConstructionView : public QWidget
{
    Q_OBJECT

public:
    /*!
    * Class constructor.
    * @param [in] parent Parent widget
    */
    ConstructionView(QWidget* parent = Q_NULLPTR);

    /*!
    * Set the construction view's reference to the system robot instance.
    */
    void setRobot(Robot* robot);

signals:
    /*!
    * Generated when a message is logged by an \class ConstructionView instance.
    */
    void log(Message message) const;

private:
    QVBoxLayout* baseLayout; /* Layout for all the main construction view components and layouts */
    QHBoxLayout* robotPositionLayout;
    QHBoxLayout* robotControlLayout;
    QPushButton* sleepRobot;
    QPushButton* wakeRobot;
    QPushButton* calibrateRobot;
    QPushButton* setRobotPosition;
    QPushButton* idleRobotActuator;
    QPushButton* actuateRobotActuator;
    QPushButton* releaseRobotActuator;
    QPushButton* demo;
    QLabel* xPositionLabel;
    QLabel* yPositionLabel;
    QLabel* zPositionLabel;
    QLabel* rPositionLabel;
    QSpinBox* xPosition;
    QSpinBox* yPosition;
    QSpinBox* zPosition;
    QSpinBox* rPosition;
    Robot* robot = Q_NULLPTR; /*! Reference to interface with the robotic subsystem */

    void sleepRobotClicked();
    void wakeRobotClicked();
    void calibrateRobotClicked();
    void setRobotPositionClicked();
    void idleRobotActuatorClicked();
    void actuateRobotActuatorClicked();
    void releaseRobotActuatorClicked();
    void performDemo();
};
