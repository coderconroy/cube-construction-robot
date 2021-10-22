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
    shapeView = new OpenGLView();
    shapeView->setCubes(cubeWorldModel->getCubes());

    connect(shapeView, &OpenGLView::log, this, &ConstructionView::log); // Propagate log signal

    // Initialize camera feed and controls
    overviewCameraFeed = new QLabel();

    // Initialize general robot controls
    loadModel = new QPushButton("Load Model");
    execute = new QPushButton("Start Construction");
    sleepRobot = new QPushButton("Sleep");
    wakeRobot = new QPushButton("Wake");
    calibrateRobot = new QPushButton("Calibrate");
    idleRobotActuator = new QPushButton("Actuator->Idle");
    actuateRobotActuator = new QPushButton("Actuator->Actuate");
    releaseRobotActuator = new QPushButton("Actuator->Release");

    connect(loadModel, &QPushButton::clicked, this, &ConstructionView::loadModelClicked);
    connect(execute, &QPushButton::clicked, this, &ConstructionView::executeConstruction);
    connect(sleepRobot, &QPushButton::clicked, this, &ConstructionView::sleepRobotClicked);
    connect(wakeRobot, &QPushButton::clicked, this, &ConstructionView::wakeRobotClicked);
    connect(calibrateRobot, &QPushButton::clicked, this, &ConstructionView::calibrateRobotClicked);
    connect(idleRobotActuator, &QPushButton::clicked, this, &ConstructionView::idleRobotActuatorClicked);
    connect(actuateRobotActuator, &QPushButton::clicked, this, &ConstructionView::actuateRobotActuatorClicked);
    connect(releaseRobotActuator, &QPushButton::clicked, this, &ConstructionView::releaseRobotActuatorClicked);

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

    // Initialize camera overview layout
    overviewCameraLayout = new QVBoxLayout();
    overviewCameraLayout->addWidget(overviewCameraFeed);

    // Intialize shape overview layout
    overviewModelLayout = new QVBoxLayout();
    overviewModelLayout->addWidget(shapeView);

    // Initialize visual layout
    visualLayout = new QHBoxLayout();
    visualLayout->addLayout(overviewCameraLayout);
    visualLayout->addLayout(overviewModelLayout);

    // Initialize overview layout
    overviewLayout = new QVBoxLayout();
    overviewLayout->addStretch();
    overviewLayout->addLayout(visualLayout);
    overviewLayout->addSpacing(20);
    overviewLayout->addLayout(robotControlLayout);
    overviewLayout->addSpacing(20);
    overviewLayout->addLayout(robotPositionLayout);
    overviewLayout->addStretch();

    overviewWidget = new QWidget();
    overviewWidget->setLayout(overviewLayout);

    // Initialize computer vision controls
    visionStageGroup = new QButtonGroup(this);
    visionInput = new QRadioButton("Camera Feed");
    visionBlurred = new QRadioButton("Grayscale and Blurring");
    visionThreshold = new QRadioButton("Thresholding");
    visionContours = new QRadioButton("Contour Detection");
    visionFiducials = new QRadioButton("Fiducial Processing");
    boundingBox = new QCheckBox("Bounding Box");
    fiducialInfo = new QCheckBox("Fiducial Info");
    cubeInfo = new QCheckBox("Cube Info");
    worldPointXPos = new QSpinBox();
    worldPointYPos = new QSpinBox();
    worldPointZPos = new QSpinBox();
    projectWorldPoint = new QPushButton("Project point");

    visionStageGroup->addButton(visionInput);
    visionStageGroup->addButton(visionBlurred);
    visionStageGroup->addButton(visionThreshold);
    visionStageGroup->addButton(visionContours);
    visionStageGroup->addButton(visionFiducials);

    // Initialize comptuer vision controls layout
    visionControls = new QVBoxLayout();
    visionControls->addWidget(visionInput);
    visionControls->addWidget(visionBlurred);
    visionControls->addWidget(visionThreshold);
    visionControls->addWidget(visionContours);
    visionControls->addWidget(visionFiducials);
    visionControls->addWidget(boundingBox);
    visionControls->addWidget(fiducialInfo);
    visionControls->addWidget(cubeInfo);
    visionControls->addWidget(worldPointXPos);
    visionControls->addWidget(worldPointYPos);
    visionControls->addWidget(worldPointZPos);
    visionControls->addWidget(projectWorldPoint);

    // Initialize computer vision visual
    visionVisual = new QStackedLayout();

    // Initialize computer vision layout
    visionLayout = new QHBoxLayout();
    visionLayout->addLayout(visionControls);
    visionLayout->addLayout(visionVisual);

    visionWidget = new QWidget();
    visionWidget->setLayout(visionLayout);

    // Initialize model controls and view
    modelInputGroup = new QButtonGroup();
    showBuildModel = new QRadioButton("Shape Model");
    showWorldModel = new QRadioButton("Construction Visualizaton");
    modelView = new OpenGLView();

    modelInputGroup->addButton(showBuildModel);
    modelInputGroup->addButton(showWorldModel);

    // Initialize model controls layout
    modelControls = new QVBoxLayout();
    modelControls->addWidget(showBuildModel);
    modelControls->addWidget(showWorldModel);
    modelControls->addStretch();

    // Initialize model layout
    modelLayout = new QHBoxLayout();
    modelLayout->addLayout(modelControls);
    modelLayout->addWidget(modelView);

    modelWidget = new QWidget();
    modelWidget->setLayout(modelLayout);

    // Initialize base layout
    baseLayout = new QStackedLayout();

    baseLayout->addWidget(visionWidget);
    baseLayout->addWidget(modelWidget);
    // NOTE: The program throws a debug error on close when the overviewWidget is added to the stackWidget before the model widget
    // The cause of the issue is likely related to the OpenGL components both widgets contain
    baseLayout->addWidget(overviewWidget);

    baseLayout->setCurrentWidget(overviewWidget);

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
    cameraFeedTimer->start(500); // Update camera feed every 20ms
    openGLTimer->start(2); // Refresh OpenGL render every 20ms
}

void ConstructionView::hideView()
{
    cameraFeedTimer->stop(); // Do not refresh camera feed when the construction view is hidden
    openGLTimer->stop(); // Do not refresh OpenGL render when the design view is hidden
}

void ConstructionView::updateShapeView()
{
    if (baseLayout->currentWidget() == overviewWidget)
        shapeView->update();
    else if (baseLayout->currentWidget() == modelWidget)
        modelView->update();
}

void ConstructionView::updateCameraFeed()
{
    if (baseLayout->currentWidget() == overviewWidget)
    {
        // Capture frame from camera
        cv::Mat input;
        *camera >> input;

        cv::Mat output;
        input.copyTo(output);

        Vision vision;
        vision.calibrate(input);
        vision.processScene(input);
        vision.plotBoundingBox(output);
        vision.plotFiducialInfo(output);
        vision.plotCubeInfo(output);

        cv::resize(output, output, cv::Size(), 0.5, 0.5);
        //cv::imwrite("output1.jpg", output);

        // Display image in camera feed
        cvtColor(output, output, cv::COLOR_BGR2RGB); // Convert from BGR to RGB
        QImage cameraFeedImage = QImage((uchar*)output.data, output.cols, output.rows, output.step, QImage::Format_RGB888);
        overviewCameraFeed->setPixmap(QPixmap::fromImage(cameraFeedImage));
    }
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

const int numCubes = 17;
int xSrc[numCubes] = { 1000, 937, 873, 810, 747, 683, 620, 1000, 937, 873, 810, 747, 683, 620, 1000, 937, 873 };
int ySrc[numCubes] = { 1110, 1110, 1110, 1109, 1109, 1109, 1109, 1046, 1046, 1046, 1045, 1045, 1045, 1045, 983, 983, 983 };
int zSrc[numCubes] = { 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180 };
int xOffset = 500;
int yOffset = 500;
int cubeHeight = 318; // Vertical steps
int cubeSideLength = 100;  // Horizontal steps

void ConstructionView::executeConstruction()
{
    for (int i = 0; i < cubeTasks.size(); ++i)
        delete cubeTasks[i];
    cubeTasks.clear();

    // TODO: Sort cubes
    for (int i = 0; i < cubeWorldModel->getCubeCount(); ++i) {
        Cube* cube = cubeWorldModel->getCubes()->at(i);
        glm::vec3 destPos = cube->getPosition();
        int zRotation = glm::degrees(cube->getPitch()) / 1.8;

        CubeTask* task = new CubeTask();
        task->setSourcePose(xSrc[i], ySrc[i], zSrc[i], 0);
        task->setDestinationPose(destPos.x + xOffset, destPos.z  + yOffset, destPos.y * 318 / 64, zRotation);
        cubeTasks.append(task);
    }

    cubeTasks.first()->performNextStep(robot);
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

void ConstructionView::handleRobotCommand()
{
    emit log(Message(MessageType::INFO_LOG, "Construction view", "Command Complete"));

    if (!cubeTasks.isEmpty())
    {
        CubeTask* task = cubeTasks.first();

        if (!task->isComplete())
        {
            task->performNextStep(robot);
        }
        else
        {
            delete cubeTasks.first();
            cubeTasks.removeFirst();
            task = Q_NULLPTR;

            if (cubeTasks.size() > 0)
                cubeTasks.first()->performNextStep(robot);
        }
    }
}