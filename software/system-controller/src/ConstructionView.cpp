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
    cameraFeedTimer->start(1000); // Update camera feed every 20ms
    openGLTimer->start(2); // Refresh OpenGL render every 20ms
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

// Threshold parameters
int thresh = 80;
int maxThresh = 255;
int blurSize = 1;
int maxBlurSize = 20;

// Bounding box parameters
int left = 480;
int right = 1360;
int top = 150;
int bottom = 950;
bool showCoords = true;
double cameraMat[3][3] = 
    {{696.2920653066839 * 2, 0, 469.7644569362635 * 2},
    {0, 696.1538823160478 * 2, 281.0969237061734 * 2},
    {0, 0, 1}};
cv::Point2d principlePoint(469.7644569362635 * 2, 281.0969237061734 * 2);
double areaThreshold = 1300;


void ConstructionView::updateCameraFeed()
{
    // Capture frame from camera
    cv::Mat input;
    *camera >> input;
    
    cv::Mat frame;
    input.copyTo(frame);

    // Process image
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    cv::blur(frame, frame, cv::Size(blurSize + 1, blurSize + 1));
    cv::threshold(frame, frame, thresh, maxThresh, cv::THRESH_BINARY);

    // Apply contour detection
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(frame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Get the moment of each contour
    std::vector<cv::Moments> contourMoments(contours.size());
    for (int i = 0; i < contours.size(); i++)
        contourMoments[i] = moments(contours[i], false);

    // Get the centroid of each contour moment
    std::vector<cv::Point2f> momentCentroids(contours.size());
    for (int i = 0; i < contours.size(); i++)
        momentCentroids[i] = cv::Point2f(contourMoments[i].m10 / contourMoments[i].m00, contourMoments[i].m01 / contourMoments[i].m00);

    // Draw and label each moment centroid
    for (int i = 0; i < contours.size(); i++)
    {
        cv::Point2d centroid = momentCentroids[i];
        double area = cv::contourArea(contours[i]);
        if (centroid.x > left && centroid.x < right && centroid.y > top && centroid.y < bottom && area > areaThreshold)
        {
            circle(input, momentCentroids[i], 4, cv::Scalar(255, 0, 0), -1, cv::LINE_AA);
            std::vector<std::vector<cv::Point>> contour;
            contour.push_back(contours[i]);

            cv::drawContours(input, contour, -1, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

            double cx = momentCentroids[i].x;
            double cy = momentCentroids[i].y;
            double maxDist = 0;
            double maxIndex = 0;
            for (int j = 0; j < contours[i].size(); j++)
            {
                double px = contours[i][j].x;
                double py = contours[i][j].y;
                double dist = sqrt(pow(px - cx, 2) + pow(py - cy, 2));

                if (dist > maxDist)
                {
                    maxDist = dist;
                    maxIndex = j;
                }
            }

            circle(input, contours[i][maxIndex], 4, cv::Scalar(0, 255, 255), -1, cv::LINE_AA);

            if (showCoords)
            {
                cv::putText(input,
                    "(" + std::to_string((int)momentCentroids[i].x) + ", " + std::to_string((int)momentCentroids[i].y) + ")",
                    cv::Point2f(momentCentroids[i].x - 90, momentCentroids[i].y + 50), // Coordinates
                    cv::FONT_HERSHEY_PLAIN, // Font
                    2, // Scale. 2.0 = 2x bigger
                    cv::Scalar(255, 255, 255), // BGR Color
                    2, // Line Thickness (Optional)
                    cv::LINE_AA); // Anti-alias (Optional, see version note)


            }
        }
    }

    // Show principle point
    circle(input, principlePoint, 4, cv::Scalar(255, 0, 255), -1, cv::LINE_AA);

    // Draw image box
    cv::Scalar color(255, 255, 0);
    cv::Point tl(left, top), bl(left, bottom), tr(right, top), br(right, bottom);
    int thickness = 2;

    // Line drawn using 8 connected
    // Bresenham algorithm
    line(input, tl, tr, color, thickness, cv::LINE_8);
    line(input, tr, br, color, thickness, cv::LINE_8);
    line(input, br, bl, color, thickness, cv::LINE_8);
    line(input, bl, tl, color, thickness, cv::LINE_8);

    cv::Mat output;
    cv::resize(input, output, cv::Size(), 0.75, 0.75);
    //cv::imwrite("output1.jpg", input);

    // Display image in camera feed
    cvtColor(output, output, cv::COLOR_BGR2RGB); // Convert from BGR to RGB
    QImage cameraFeedImage = QImage((uchar*)output.data, output.cols, output.rows, output.step, QImage::Format_RGB888);
    cameraFeed->setPixmap(QPixmap::fromImage(cameraFeedImage));
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