#include "ConstructionView.h"
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QList>

QVector<Cube*> sourceCubes;
QVector<Cube*> structCubes;
int missingCubes = 0;
int externalCubes = 0;

ConstructionView::ConstructionView(QWidget* parent): QWidget(parent)
{
    // Initialize cube world model
    cubeBuildModel = new CubeWorldModel(64, 10, this);
    cubeWorldModel = new CubeWorldModel(64, 10, this);

    // Propagate log signals
    connect(cubeBuildModel, &CubeWorldModel::log, this, &ConstructionView::log);
    connect(cubeWorldModel, &CubeWorldModel::log, this, &ConstructionView::log);

    // Initialize OpenGL view for the 3D shapes
    shapeView = new OpenGLView();
    shapeView->setCubes(cubeBuildModel->getCubes());

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

    showBuildModel->setChecked(true);
    modelView->setCubes(cubeBuildModel->getCubes());

    modelBack->setMaximumWidth(170);
    showBuildModel->setMaximumWidth(170);
    showWorldModel->setMaximumWidth(170);

    connect(modelBack, &QPushButton::clicked, this, &ConstructionView::modelBackClicked);
    connect(modelInputGroup, &QButtonGroup::buttonToggled, this, &ConstructionView::modelInputUpdate);

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

    // Initialize source cubes in the world space model
    sourceCubes.append(cubeWorldModel->insertCube(825, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(762, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(699, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(635, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(572, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(509, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(446, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(383, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(320, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(256, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(193, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(130, 32, 16, 0));
    sourceCubes.append(cubeWorldModel->insertCube(825, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(762, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(699, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(635, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(572, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(509, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(446, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(383, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(320, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(256, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(193, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(130, 32, 79, 0));
    sourceCubes.append(cubeWorldModel->insertCube(825, 32, 143, 0));
    sourceCubes.append(cubeWorldModel->insertCube(762, 32, 143, 0));
    sourceCubes.append(cubeWorldModel->insertCube(699, 32, 143, 0));
    sourceCubes.append(cubeWorldModel->insertCube(635, 32, 143, 0));
}

void ConstructionView::showView()
{
    shapeView->show();
    cameraFeedTimer->start(200); // Update camera feed every 20ms
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
        vision.plotSourceCubeInfo(output);
        vision.plotStructCubeInfo(output);
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
        cubeBuildModel->read(document.object());
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

void ConstructionView::modelInputUpdate(QAbstractButton* button, bool checked)
{
    // Set the list of cubes to render based on the cube model input selection
    if (showBuildModel->isChecked())
    {
        shapeView->setCubes(cubeBuildModel->getCubes());
        modelView->setCubes(cubeBuildModel->getCubes());
    }
    else if (showWorldModel->isChecked())
    {
        shapeView->setCubes(cubeWorldModel->getCubes());
        modelView->setCubes(cubeWorldModel->getCubes());
    }

}

void ConstructionView::executeConstruction()
{
    // Clear any incomplete cube tasks from the previous construction event
    for (int i = 0; i < cubeTasks.size(); ++i)
        delete cubeTasks[i];
    cubeTasks.clear();

    // Copy list and sort cubes by z, x, y values respectively
    QList<Cube*> cubes;
    for (int i = 0; i < cubeBuildModel->getCubeCount(); ++i) {
        Cube* cube = cubeBuildModel->getCubes()->at(i);
        bool inserted = false;
        for (int j = 0; j < cubes.size(); ++j)
        {
            // Initialize the position comparision variables
            int xCube = round(cube->getPosition().x);
            int yCube = round(cube->getPosition().y);
            int zCube = round(cube->getPosition().z);

            int xList = round(cubes[j]->getPosition().x);
            int yList = round(cubes[j]->getPosition().y);
            int zList = round(cubes[j]->getPosition().z);
                
            // Insert in place of list cube if placed earlier in the construction process
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

        // Append to the end of the list if the cube does not precede any existing cubes in the construction process
        if (inserted == false)
            cubes.append(cube);
    }

    // Generate cube tasks
    for (int i = 0; i < cubes.size(); ++i) {
        CubeTask* task = new CubeTask();
        task->setDestinationCube(cubes[i]);
        cubeTasks.append(task);
    }

    // Move robot to initial position
    robot->setPosition(0, 0, 1000, 0);

    // Initiate pressure sensor requests
    pressureTimer->start(50); // Request pressure every 50 ms
}

void ConstructionView::setRobot(Robot* robot)
{
    // Initialize robot reference and signal connections
    this->robot = robot;
    connect(robot, &Robot::commandCompleted, this, &ConstructionView::handleRobotCommand);
    connect(robot, &Robot::pressureUpdated, this, &ConstructionView::pressureUpdated);
}


enum class ConstructionState
{
    CONSTRUCT_TASK,
    CONSTRUCT_VISION
};

int pressureThreshold = 500;
ConstructionState constructState = ConstructionState::CONSTRUCT_VISION;

void ConstructionView::handleRobotCommand()
{
    CubeTask* task = Q_NULLPTR;

    // Check if there are any cube tasks to be performed
    if (cubeTasks.isEmpty())
        return;

    switch (constructState)
    {
    case ConstructionState::CONSTRUCT_TASK:

        task = cubeTasks.first();

        // Check if task complete
        if (task->isComplete())
        {
            // Add source cube to list of sucessfully placed cubes in the 3D shape structure
            structCubes.append(task->getSourceCube());

            // Remove completed cube task from the list of incomplete cube tasks
            delete cubeTasks.first();
            cubeTasks.removeFirst();
            task = Q_NULLPTR;

            // Check if construction is complete
            if (cubeTasks.isEmpty())
            {
                // Move robot to xy origin
                robot->setPosition(0, 0, robot->getZPosition(), 0);

                // Stop the pressure sensor reading requests
                pressureTimer->stop();
            }
            else
            {
                // Activate the computer vision phase of the construction state to detect the cube
                constructState = ConstructionState::CONSTRUCT_VISION;
                handleRobotCommand();
            }

            return;
        }

        // Initialize source position if task has not been started
        if (!task->isStarted())
        {
            // Check if any source cubes remain
            if (sourceCubes.size() > 0)
            {
                task->setSourceCube(sourceCubes.takeFirst());
            }
            else
            {
                emit log(Message(MessageType::ERROR_LOG, "Construction View", "No source cubes remain to build the structure"));
                return;
            }
        }

        // Check if the cube is not gripped when the task step expects it to be gripped
        if (task->expectGrippedCube() && robot->getPressure() < pressureThreshold)
        {
            emit log(Message(MessageType::INFO_LOG, "Construction", "Failed to grip the cube"));

            // Activate the computer vision phase of the construction state to detect the cube
            missingCubes++;
            constructState = ConstructionState::CONSTRUCT_VISION;

            // Update the status of the failed source cube in the cube world model
            task->getSourceCube()->setState(CubeState::INVALID);

            // Restart the task
            task->resetSteps(robot);
            return;
        }

        // Set the source cube position in the cube world model to that of the robot if the cube is gripped
        // The coodinates are converted from the robot coordinate system to the OpenGL coordinate system
        if (robot->getPressure() >= pressureThreshold)
        {
            glm::vec3 position(robot->getXPosition(), robot->getZPosition() * 64 / 318 - 32, robot->getYPosition());
            glm::vec3 orientation(0, glm::radians(robot->getRPosition() * 1.8), 0);
            task->getSourceCube()->setPosition(position);
            task->getSourceCube()->setOrientation(orientation);
        }

        // Instruct the robot to perform the next step in the task
        task->performNextStep(robot);

        break;

    case ConstructionState::CONSTRUCT_VISION:
        // Verify robot is in the correct position to ensure there are no occlusions
        if (robot->getXPosition() != 507 || robot->getYPosition() != 0 || robot->getZPosition() != 2000)
        {
            // Instruct robot to go to computer vison position
            robot->setPosition(507, 0, 2000, 0);
            return;
        }

        emit log(Message(MessageType::INFO_LOG, "Construction", "Processing image..."));

        // Create list of source cube top face centroids excluding source cube being processed by current task
        std::vector<cv::Point3i> sourceCentroids;
        for (int i = 0; i < sourceCubes.size(); ++i)
        {
            // Get cube position in the OpenGL coordinate system
            glm::vec3 cubePos = sourceCubes[i]->getPosition();

            // Convert position to top face centroid position in robot coordinate system
            cv::Point3i centroid(cubePos.x, cubePos.z, cubePos.y + 32);
            sourceCentroids.push_back(centroid);
        }

        // Create list of successfully placed structure cube top face centroids
        std::vector<cv::Point3i> structCentroids;
        for (int i = 0; i < structCubes.size(); ++i)
        {
            // Get cube position in the OpenGL coordinate system
            glm::vec3 cubePos = structCubes[i]->getPosition();

            // Convert position to top face centroid position in robot coordinate system
            cv::Point3i centroid(cubePos.x, cubePos.z, cubePos.y + 32);
            structCentroids.push_back(centroid);
        }

        // Capture frame from camera
        cv::Mat input;
        *camera >> input;

        // Process image
        vision.processScene(input, true, &sourceCentroids, &structCentroids);

        // Analyze cubes detected in the workspace that are not part of the source cubes or 3D shape structure
        std::vector<cv::Point3i> detectedCubes = vision.getCubeCentroids(64);
        if (detectedCubes.size() > 0)
        {
            // Check if the construction has failed
            // This is assumed when there are more independent cubes detected than expected
            int expectedCubes = missingCubes + externalCubes;
            if (detectedCubes.size() > expectedCubes)
            {
                // TODO: Reset construction state
                emit log(Message(MessageType::INFO_LOG, "Construction", "Construction failure detected"));
                cubeTasks.clear();
                break;
            }
            // The missing cube has been detected in the workspace
            else if (detectedCubes.size() == expectedCubes)
            {
                emit log(Message(MessageType::INFO_LOG, "Construction", "Missing cube detected"));

                // Update the position of the missing cube
                glm::vec3 detectedPos(detectedCubes[0].x, detectedCubes[0].z - 32, detectedCubes[0].y);
                task = cubeTasks.first();
                task->getSourceCube()->setPosition(detectedPos);

                // Use the detected missing cube as the next source cube
                sourceCubes.insert(0, task->getSourceCube());
            }
        }

        // Update state to indicate missing cube has been processed if cube was missing
        if (missingCubes > 0)
            missingCubes--;


        // Activate the cube task execution phase of the construction state to place the cube
        constructState = ConstructionState::CONSTRUCT_TASK;
        handleRobotCommand();
        break;
    }
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