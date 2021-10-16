#include "ConstructionView.h"
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QList>

ConstructionView::ConstructionView(QWidget* parent): QWidget(parent)
{
    // Initialize cube world model
    cubeWorldModel = new CubeWorldModel(64, 10, this);
    connect(cubeWorldModel, &CubeWorldModel::log, this, &ConstructionView::log); // Propagate log signal

    // Initialize OpenGL view for the 3D shapes
    shapeView = new OpenGLView(parent);
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    shapeView->setFormat(format);
    shapeView->setCubes(cubeWorldModel->getCubes());

    connect(shapeView, &OpenGLView::log, this, &ConstructionView::log); // Propagate log signal

    // Initialize camera feed and controls
    cameraFeed = new QLabel();

    // Initialize general robot controls
    loadModel = new QPushButton("Load Model");
    execute = new QPushButton("Start Construction");
    sleepRobot = new QPushButton("Sleep");
    wakeRobot = new QPushButton("Wake");
    calibrateRobot = new QPushButton("Calibrate");
    idleRobotActuator = new QPushButton("Actuator->Idle");
    actuateRobotActuator = new QPushButton("Actuator->Actuate");
    releaseRobotActuator = new QPushButton("Actuator->Release");
    demo = new QPushButton("Perform Demo");

    connect(loadModel, &QPushButton::clicked, this, &ConstructionView::loadModelClicked);
    connect(sleepRobot, &QPushButton::clicked, this, &ConstructionView::sleepRobotClicked);
    connect(wakeRobot, &QPushButton::clicked, this, &ConstructionView::wakeRobotClicked);
    connect(calibrateRobot, &QPushButton::clicked, this, &ConstructionView::calibrateRobotClicked);
    connect(idleRobotActuator, &QPushButton::clicked, this, &ConstructionView::idleRobotActuatorClicked);
    connect(actuateRobotActuator, &QPushButton::clicked, this, &ConstructionView::actuateRobotActuatorClicked);
    connect(releaseRobotActuator, &QPushButton::clicked, this, &ConstructionView::releaseRobotActuatorClicked);
    connect(demo, &QPushButton::clicked, this, &ConstructionView::performDemo);

    robotControlLayout = new QHBoxLayout();
    robotControlLayout->addStretch();
    robotControlLayout->addWidget(loadModel);
    robotControlLayout->addWidget(execute);
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

    // Initialize camera layout
    cameraLayout = new QVBoxLayout();
    cameraLayout->addWidget(cameraFeed);

    // Intialize shape layout
    shapeLayout = new QVBoxLayout();
    shapeLayout->addWidget(shapeView);

    // Initialize visual layout
    visualLayout = new QHBoxLayout();
    visualLayout->addLayout(cameraLayout);
    visualLayout->addLayout(shapeLayout);

    // Initialize base layout
    baseLayout = new QVBoxLayout();
    baseLayout->addStretch();
    baseLayout->addLayout(visualLayout);
    baseLayout->addSpacing(20);
    baseLayout->addLayout(robotControlLayout);
    baseLayout->addSpacing(20);
    baseLayout->addLayout(robotPositionLayout);
    baseLayout->addStretch();

    setLayout(baseLayout);

    // Initialize camera feed timer
    cameraFeedTimer = new QTimer(this);
    connect(cameraFeedTimer, &QTimer::timeout, this, &ConstructionView::updateCameraFeed);

    // Initialize OpenGL shape view timer
    openGLTimer = new QTimer(this);
    connect(openGLTimer, &QTimer::timeout, this, &ConstructionView::updateShapeView);
}

void ConstructionView::showView()
{
    cameraFeedTimer->start(20); // Update camera feed every 20ms
    openGLTimer->start(20); // Refresh OpenGL render every 20ms
}

void ConstructionView::hideView()
{
    cameraFeedTimer->stop(); // Do not refresh camera feed when the construction view is hidden
    openGLTimer->stop(); // Do not refresh OpenGL render when the design view is hidden
}

void ConstructionView::updateShapeView()
{
    shapeView->update();
}

void ConstructionView::loadModelClicked()
{
    // Read JSON cube world model from file
    QFile jsonFile("models/testFile.cubeworld");
    if (jsonFile.open(QIODevice::ReadOnly))
    {
        // Read file into byte array
        QByteArray jsonBytes = jsonFile.readAll();
        jsonFile.close();

        // Parse JSON document from byte array
        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson(jsonBytes, &jsonError);
        if (jsonError.error != QJsonParseError::NoError)
        {
            QString errorMessage = QString("Failed to read from JSON cube world model file: ") + jsonError.errorString();
            emit log(Message(MessageType::ERROR_LOG, "Design View", errorMessage));
            return;
        }

        // Initialize cube world model with JSON object
        cubeWorldModel->read(document.object());
        emit log(Message(MessageType::INFO_LOG, "Construction View", "Cube world model successfully loaded from file"));
    }
}

void ConstructionView::executeConstruction()
{

}

void ConstructionView::setRobot(Robot* robot)
{
    this->robot = robot;
    connect(robot, &Robot::commandCompleted, this, &ConstructionView::handleRobotCommand);
}

void ConstructionView::setCamera(cv::VideoCapture* camera)
{
    this->camera = camera;
}

void ConstructionView::updateCameraFeed()
{
    // Capture frame from camera
    cv::Mat frame;
    *camera >> frame;

    // Display image in camera feed
    cvtColor(frame, frame, cv::COLOR_BGR2RGB); // Convert from BGR to RGB
    QImage cameraFeedImage = QImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    cameraFeed->setPixmap(QPixmap::fromImage(cameraFeedImage));
}

void ConstructionView::handleRobotCommand()
{
    emit log(Message(MessageType::INFO_LOG, "Construction view", "Command Complete"));
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
        CubeTask task;
        task.setSourcePose(xSrc[i], ySrc[i], zSrc[i], 0);
        task.setDestinationPose(xDest[i], yDest[i], zDest[i], rDest[i]);
        
        while (!task.isComplete())
            task.performNextStep(robot);

        //// Pick up cube at source 
        //robot->setPosition(xSrc[i], ySrc[i], zSrc[i] + moveOffset, 0);
        //robot->setPosition(xSrc[i], ySrc[i], zSrc[i] - bufferAction, 0);
        //robot->actuateGripper();
        //robot->delay();
        //robot->setPosition(xSrc[i], ySrc[i], zDest[i] + moveOffset + cubeHeight, 0);

        //// Place cube at destination
        //robot->setPosition(xDest[i], yDest[i], zDest[i] + moveOffset + cubeHeight, rDest[i]);
        //robot->setPosition(xDest[i], yDest[i], zDest[i] - bufferAction, rDest[i]);
        //robot->releaseGripper();
        //robot->delay();
        //robot->setPosition(xDest[i], yDest[i], zDest[i] + moveOffset, rDest[i]);
        //robot->resetGripper();
    }
}