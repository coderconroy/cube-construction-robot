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
    showVisionView = new QPushButton("Show Vision View");
    showModelView = new QPushButton("Show Model View");
    loadModel = new QPushButton("Load Model");
    execute = new QPushButton("Start Construction");
    sleepRobot = new QPushButton("Sleep");
    wakeRobot = new QPushButton("Wake");
    calibrateRobot = new QPushButton("Calibrate");
    idleRobotActuator = new QPushButton("Actuator->Idle");
    actuateRobotActuator = new QPushButton("Actuator->Actuate");
    releaseRobotActuator = new QPushButton("Actuator->Release");

    connect(showVisionView, &QPushButton::clicked, this, &ConstructionView::showVisionViewClicked);
    connect(showModelView, &QPushButton::clicked, this, &ConstructionView::showModelViewClicked);
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
    robotControlLayout->addWidget(showVisionView);
    robotControlLayout->addWidget(showModelView);
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
    pressureLabel = new QLabel("Pressure: No Reading");
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
    robotPositionLayout->addWidget(pressureLabel);
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
    visionBack = new QPushButton("Back");
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

    visionInput->setChecked(true);

    worldPointXPos->setMaximumWidth(100);
    worldPointYPos->setMaximumWidth(100);
    worldPointZPos->setMaximumWidth(100);
    projectWorldPoint->setMaximumWidth(100);

    connect(visionBack, &QPushButton::clicked, this, &ConstructionView::visionBackClicked);

    // Initialize comptuer vision controls layout
    visionControls = new QVBoxLayout();
    visionControls->addStretch();
    visionControls->addWidget(visionBack);
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
    visionControls->addStretch();

    // Initialize computer vision visual
    visionImage = new QLabel();

    // Initialize computer vision layout
    visionLayout = new QHBoxLayout();
    visionLayout->addLayout(visionControls);
    visionLayout->addWidget(visionImage);

    visionWidget = new QWidget();
    visionWidget->setLayout(visionLayout);

    // Initialize model controls and view
    modelBack = new QPushButton("Back");
    modelInputGroup = new QButtonGroup();
    showBuildModel = new QRadioButton("Shape Model");
    showWorldModel = new QRadioButton("Construction Visualizaton");
    modelView = new OpenGLView();

    modelInputGroup->addButton(showBuildModel);
    modelInputGroup->addButton(showWorldModel);

    modelBack->setMaximumWidth(170);
    showBuildModel->setMaximumWidth(170);
    showWorldModel->setMaximumWidth(170);

    connect(modelBack, &QPushButton::clicked, this, &ConstructionView::modelBackClicked);

    // Initialize model controls layout
    modelControls = new QVBoxLayout();
    modelControls->addWidget(modelBack);
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
    // UPDATE: The cause appears to be the if the shapeView is added to the widget node graph, it must be displayed on screen before
    // the program terminates to prevent the debug error. Hiding the OpenGL widget before it is added to the widget graph and only
    // showing it when the screen is displayed seems to fix the issue.
    shapeView->hide();
    baseLayout->addWidget(overviewWidget);

    baseLayout->setCurrentWidget(overviewWidget);

    setLayout(baseLayout);

    // Initialize camera feed timer
    cameraFeedTimer = new QTimer(this);
    connect(cameraFeedTimer, &QTimer::timeout, this, &ConstructionView::updateCameraFeed);

    // Initialize OpenGL shape view timer
    openGLTimer = new QTimer(this);
    connect(openGLTimer, &QTimer::timeout, this, &ConstructionView::updateShapeView);

    // Initialize robot pressure reading request timer
    pressureTimer = new QTimer(this);
    connect(pressureTimer, &QTimer::timeout, this, &ConstructionView::requestPressureUpdate);
}

void ConstructionView::showView()
{
    shapeView->show();
    cameraFeedTimer->start(500); // Update camera feed every 20ms
    openGLTimer->start(20); // Refresh OpenGL render every 20ms
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

void ConstructionView::requestPressureUpdate()
{
    robot->requestPressure();
}

void ConstructionView::pressureUpdated()
{
    pressureLabel->setText("Pressure: " + QString::number(robot->getPressure()));
}

void ConstructionView::updateCameraFeed()
{
    QLabel* display;
    float scaleFactor;
    if (baseLayout->currentWidget() == overviewWidget)
    {
        display = overviewCameraFeed;
        scaleFactor = 0.4;
    }
    else if (baseLayout->currentWidget() == visionWidget)
    {
        display = visionImage;
        scaleFactor = 0.8;
    }
    else
    {
        return;
    }

    // Capture frame from camera
    cv::Mat input;
    *camera >> input;

    // Process input
    Vision vision;
    vision.calibrate(input);
    vision.processScene(input);

    // Select image to display based on user vision stage selection
    cv::Mat output;
    if (visionInput->isChecked())
    {
        input.copyTo(output);
        if (boundingBox->isChecked())
            vision.plotBoundingBox(output);
        if (fiducialInfo->isChecked())
            vision.plotFiducialInfo(output);
        if (cubeInfo->isChecked())
            vision.plotCubeInfo(output);
    }
    else if (visionBlurred->isChecked())
    {
        output = vision.getBlurredImage();
    }
    else if (visionThreshold->isChecked())
    {
        output = vision.getThresholdedImage();
    }
    else if (visionContours->isChecked())
    {
        output = vision.getContourImage();
    }
    else if (visionFiducials->isChecked())
    {
        if (vision.getFiducialImages().size() > 0) {
            output = vision.getFiducialImages()[0];

            for (int i = 1; i < vision.getFiducialImages().size(); ++i)
                cv::hconcat(output, vision.getFiducialImages()[i], output);
        }
    }

    // Resize image for display label
    if (output.size().height > 0 && output.size().width > 0)
    {
        if (visionFiducials->isChecked())
            cv::resize(output, output, cv::Size(), 1, 1);
        else
            cv::resize(output, output, cv::Size(), scaleFactor, scaleFactor);

        // Display image
        cvtColor(output, output, cv::COLOR_BGR2RGB); // Convert from BGR to RGB
        QImage outputImage = QImage((uchar*)output.data, output.cols, output.rows, output.step, QImage::Format_RGB888);
        display->setPixmap(QPixmap::fromImage(outputImage));
    }
}

void ConstructionView::showVisionViewClicked()
{
    baseLayout->setCurrentWidget(visionWidget);
}

void ConstructionView::showModelViewClicked()
{
    baseLayout->setCurrentWidget(modelWidget);
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

void ConstructionView::visionBackClicked()
{
    baseLayout->setCurrentWidget(overviewWidget);
}

void ConstructionView::modelBackClicked()
{
    baseLayout->setCurrentWidget(overviewWidget);
}

const int numCubes = 30;
int xSrc[numCubes] = {825, 762, 699, 635, 572, 509, 446, 383, 320, 256, 193, 130,
                      825, 762, 699, 635, 572, 509, 446, 383, 320, 256, 193, 130,
                      825, 762, 699, 635, 572, 509};
int ySrc[numCubes] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      63, 63, 63, 63, 63, 63, 63, 63 ,63, 63, 63, 63,
                      127, 127, 127, 127, 127, 127};
int zSrc[numCubes] = {160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 
                      160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 
                      160, 160, 160, 160, 160, 160, 160, 160, 160, 160};
int xOffset = 600;
int yOffset = 800;
int cubeHeight = 318; // Vertical steps
int cubeSideLength = 100;  // Horizontal steps

void ConstructionView::executeConstruction()
{
    for (int i = 0; i < cubeTasks.size(); ++i)
        delete cubeTasks[i];
    cubeTasks.clear();

    // Copy list and sort cubes by z, x, y values respectively
    QList<Cube*> cubes;
    for (int i = 0; i < cubeWorldModel->getCubeCount(); ++i) {
        Cube* cube = cubeWorldModel->getCubes()->at(i);
        bool inserted = false;
        for (int j = 0; j < cubes.size(); ++j)
        {
            int xCube = round(cube->getPosition().x);
            int yCube = round(cube->getPosition().y);
            int zCube = round(cube->getPosition().z);

            int xList = round(cubes[j]->getPosition().x);
            int yList = round(cubes[j]->getPosition().y);
            int zList = round(cubes[j]->getPosition().z);
                
            if (yCube < yList)
            {
                cubes.insert(j, cube);
                inserted = true;
                break;
            }
            else if (yCube == yList && zCube > zList)
            {
                cubes.insert(j, cube);
                inserted = true;
                break;
            }
            else if (yCube == yList && zCube == zList && xCube < xList)
            {
                cubes.insert(j, cube);
                inserted = true;
                break;
            }
        }
        if (inserted == false)
            cubes.append(cube);
    }

    // Generate cube tasks
    for (int i = 0; i < cubes.size(); ++i) {
        Cube* cube = cubes[i];
        glm::vec3 destPos = cube->getPosition();
        int zRotation = glm::degrees(cube->getPitch()) / 1.8;

        CubeTask* task = new CubeTask();
        task->setSourcePose(xSrc[i], ySrc[i], zSrc[i], 0);
        task->setDestinationPose(destPos.x + xOffset, destPos.z  + yOffset, destPos.y * 318 / 64, zRotation);
        cubeTasks.append(task);
    }

    // Move robot to initial position
    robot->setPosition(0, 0, 1000, 0);

    // Initiate pressure sensor requests
    pressureTimer->start(50); // Request pressure every 50 ms
}

void ConstructionView::setRobot(Robot* robot)
{
    this->robot = robot;
    connect(robot, &Robot::commandCompleted, this, &ConstructionView::handleRobotCommand);
    connect(robot, &Robot::pressureUpdated, this, &ConstructionView::pressureUpdated);
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

int pressureThreshold = 500;
void ConstructionView::handleRobotCommand()
{
    // Check if there are any cube tasks to be performed
    if (!cubeTasks.isEmpty())
    {
        CubeTask* task = cubeTasks.first();

        // Continue processing current cube task if not complete
        if (!task->isComplete())
        {
            // Check if the cube is gripped if the cube task step expects the cube to be gripped
            if (task->expectGrippedCube() && robot->getPressure() < pressureThreshold)
            {
                emit log(Message(MessageType::INFO_LOG, "Construction", "Cube grip fail"));
                task->resetSteps(robot);
            }
            else
                task->performNextStep(robot);
        }
        else
        {
            // Remove completed cube task from the list of incomplete cube tasks
            delete cubeTasks.first();
            cubeTasks.removeFirst();
            task = Q_NULLPTR;

            // Start next cube task if available
            if (cubeTasks.size() > 0)
            {
                cubeTasks.first()->performNextStep(robot);
            }
            // Construction complete
            else
            {
                // Move robot to xy origin
                robot->setPosition(0, 0, robot->getZPosition(), 0);

                // Stop pressure sensor requests
                pressureTimer->stop();
            }
        }
    }
}