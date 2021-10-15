#include "ConstructionView.h"

ConstructionView::ConstructionView(QWidget* parent): QWidget(parent)
{
    // Initialize general robot controls
    sleepRobot = new QPushButton("Sleep");
    wakeRobot = new QPushButton("Wake");
    calibrateRobot = new QPushButton("Calibrate");
    idleRobotActuator = new QPushButton("Actuator->Idle");
    actuateRobotActuator = new QPushButton("Actuator->Actuate");
    releaseRobotActuator = new QPushButton("Actuator->Release");
    demo = new QPushButton("Perform Demo");

    connect(sleepRobot, &QPushButton::clicked, this, &ConstructionView::sleepRobotClicked);
    connect(wakeRobot, &QPushButton::clicked, this, &ConstructionView::wakeRobotClicked);
    connect(calibrateRobot, &QPushButton::clicked, this, &ConstructionView::calibrateRobotClicked);
    connect(idleRobotActuator, &QPushButton::clicked, this, &ConstructionView::idleRobotActuatorClicked);
    connect(actuateRobotActuator, &QPushButton::clicked, this, &ConstructionView::actuateRobotActuatorClicked);
    connect(releaseRobotActuator, &QPushButton::clicked, this, &ConstructionView::releaseRobotActuatorClicked);
    connect(demo, &QPushButton::clicked, this, &ConstructionView::performDemo);

    robotControlLayout = new QHBoxLayout();
    robotControlLayout->addStretch();
    robotControlLayout->addWidget(sleepRobot);
    robotControlLayout->addWidget(wakeRobot);
    robotControlLayout->addWidget(calibrateRobot);
    robotControlLayout->addWidget(idleRobotActuator);
    robotControlLayout->addWidget(actuateRobotActuator);
    robotControlLayout->addWidget(releaseRobotActuator);
    robotControlLayout->addWidget(demo);
    robotControlLayout->addStretch();

    // Initialize robot position control
    xPositionLabel = new QLabel("X steps: [0, 1015]");
    yPositionLabel = new QLabel("Y steps: [0, 1125]");
    zPositionLabel = new QLabel("Z steps: [0, 2390]");
    rPositionLabel = new QLabel("R steps: [-78, 78]");
    xPosition = new QSpinBox();
    yPosition = new QSpinBox();
    zPosition = new QSpinBox();
    rPosition = new QSpinBox();
    setRobotPosition = new QPushButton("Initiate Move");

    xPosition->setMinimum(0);
    xPosition->setMaximum(1015);
    yPosition->setMinimum(0);
    yPosition->setMaximum(1125);
    zPosition->setMinimum(0);
    zPosition->setMaximum(2390);
    rPosition->setMinimum(-78);
    rPosition->setMaximum(78);

    connect(setRobotPosition, &QPushButton::clicked, this, &ConstructionView::setRobotPositionClicked);

    robotPositionLayout = new QHBoxLayout();
    robotPositionLayout->addStretch();
    robotPositionLayout->addWidget(xPositionLabel);
    robotPositionLayout->addWidget(xPosition);
    robotPositionLayout->addWidget(yPositionLabel);
    robotPositionLayout->addWidget(yPosition);
    robotPositionLayout->addWidget(zPositionLabel);
    robotPositionLayout->addWidget(zPosition);
    robotPositionLayout->addWidget(rPositionLabel);
    robotPositionLayout->addWidget(rPosition);
    robotPositionLayout->addWidget(setRobotPosition);
    robotPositionLayout->addStretch();

    // Initialize base layout
    baseLayout = new QVBoxLayout();
    baseLayout->addStretch();
    baseLayout->addLayout(robotControlLayout);
    baseLayout->addSpacing(20);
    baseLayout->addLayout(robotPositionLayout);
    baseLayout->addStretch();

    setLayout(baseLayout);
}

void ConstructionView::setRobot(Robot* robot)
{
    this->robot = robot;
}

void ConstructionView::sleepRobotClicked()
{
    robot->sleep();
}

void ConstructionView::wakeRobotClicked()
{
    robot->wake();
}

void ConstructionView::calibrateRobotClicked()
{
    robot->calibrate();
}

void ConstructionView::setRobotPositionClicked()
{
    robot->setPosition(xPosition->value(), yPosition->value(), zPosition->value(), rPosition->value());
}

void ConstructionView::idleRobotActuatorClicked()
{
    robot->resetGripper();
}

void ConstructionView::actuateRobotActuatorClicked()
{
    robot->actuateGripper();
}

void ConstructionView::releaseRobotActuatorClicked()
{
    robot->releaseGripper();
}

const int cubeHeight = 318;
const int cubeWidth = 64;
const int baseLayer = 180;
const int bufferAction = 50;
const int moveOffset = 200;
const int cubePadding = 8;
const int numCubes = 17;
int centreSpacing = cubeWidth + 2 * cubePadding;

void ConstructionView::performDemo()
{
    int xSrc[numCubes] = { 1000, 937, 873, 810, 747, 683, 620, 1000, 937, 873, 810, 747, 683, 620, 1000, 937, 873 };
    int ySrc[numCubes] = { 1110, 1110, 1110, 1109, 1109, 1109, 1109, 1046, 1046, 1046, 1045, 1045, 1045, 1045, 983, 983, 983 };
    int zSrc[numCubes] = { 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180 };

    int xDest[numCubes] = { 500, 500 + centreSpacing, 500 + 2 * centreSpacing, 500, 500 + centreSpacing, 500 + 2 * centreSpacing,
        500, 500 + centreSpacing, 500 + 2 * centreSpacing, 500 + centreSpacing / 2, 500 + 3 * centreSpacing / 2, 500 + centreSpacing / 2,
        500 + 3 * centreSpacing / 2, 500 + centreSpacing, 500 + centreSpacing, 500 + centreSpacing, 500 + centreSpacing };
    int yDest[numCubes] = { 500, 500, 500, 500 + centreSpacing, 500 + centreSpacing, 500 + centreSpacing, 500 + 2 * centreSpacing,
        500 + 2 * centreSpacing, 500 + 2 * centreSpacing, 500 + centreSpacing / 2, 500 + centreSpacing / 2, 500 + 3 * centreSpacing / 2,
        500 + 3 * centreSpacing / 2, 500 + centreSpacing, 500 + centreSpacing, 500 + centreSpacing, 500 + centreSpacing };
    int zDest[numCubes] = { baseLayer, baseLayer, baseLayer, baseLayer, baseLayer, baseLayer, baseLayer, baseLayer, baseLayer,
    baseLayer + cubeHeight, baseLayer + cubeHeight, baseLayer + cubeHeight, baseLayer + cubeHeight, baseLayer + 2 * cubeHeight,
    baseLayer + 3 * cubeHeight, baseLayer + 4 * cubeHeight, baseLayer + 5 * cubeHeight };
    int rDest[numCubes] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 40, 60 };

    for (int i = 0; i < numCubes; i++)
    {
        // Pick up cube at source 
        robot->setPosition(xSrc[i], ySrc[i], zSrc[i] + moveOffset, 0);
        robot->setPosition(xSrc[i], ySrc[i], zSrc[i] - bufferAction, 0);
        robot->actuateGripper();
        robot->delay();
        robot->setPosition(xSrc[i], ySrc[i], zDest[i] + moveOffset + cubeHeight, 0);

        // Place cube at destination
        robot->setPosition(xDest[i], yDest[i], zDest[i] + moveOffset + cubeHeight, rDest[i]);
        robot->setPosition(xDest[i], yDest[i], zDest[i] - bufferAction, rDest[i]);
        robot->releaseGripper();
        robot->delay();
        robot->setPosition(xDest[i], yDest[i], zDest[i] + moveOffset, rDest[i]);
        robot->resetGripper();
    }
}