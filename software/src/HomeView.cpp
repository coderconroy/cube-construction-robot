#include "HomeView.h"
#include "iostream"
#include "Packet.h"
#include <QSerialPort>
#include <QList>

HomeView::HomeView(QWidget* parent) : QWidget(parent)
{
    // Initialize stylesheets
    QString headingStyleSheet = 
        "font-size: 30px;"
        "font-family: Calibri, sans-serif";

    // Initialize camera feed
    cameraHeading = new QLabel("Camera");
    cameraHeading->setStyleSheet(headingStyleSheet);
    cameraFeed = new QLabel();

    // Initialize camera connection layout
    cameraLayout = new QVBoxLayout();
    cameraLayout->addWidget(cameraHeading);
    cameraLayout->addWidget(cameraFeed);

    // Initialize robotic system connection widgets
    robotHeading = new QLabel("Robot");
    robotHeading->setStyleSheet(headingStyleSheet);
    portListLabel = new QLabel("Available Ports:");
    portList = new QListWidget();
    refreshButton = new QPushButton("Refresh Ports");
    connectButton = new QPushButton("Connect to Robot");
    connectButton->setEnabled(false);
    disconnectButton = new QPushButton("Disconnect from Robot");
    
    connect(portList, &QListWidget::itemSelectionChanged, this, &HomeView::portListSelectionChange);
    connect(refreshButton, &QPushButton::clicked, this, &HomeView::refreshAvailablePorts);
    connect(connectButton, &QPushButton::clicked, this, &HomeView::connectToRobot);

    // Initialize map of ports in list to port info
    portInfoMap = new QMap<QString, QSerialPortInfo>();
    refreshAvailablePorts();

    // Initialize robotic subsystem connection layout
    robotLayout = new QVBoxLayout();
    robotLayout->setSpacing(10);
    robotLayout->addWidget(robotHeading);
    robotLayout->addWidget(portListLabel);
    robotLayout->addWidget(portList);
    robotLayout->addWidget(refreshButton);
    robotLayout->addWidget(connectButton);

    // Initialize hardware layout
    hardwareLayout = new QHBoxLayout();
    hardwareLayout->addLayout(cameraLayout);
    hardwareLayout->addSpacing(20);
    hardwareLayout->addLayout(robotLayout);

    // Initialize general robot controls
    sleepRobot = new QPushButton("Sleep");
    wakeRobot = new QPushButton("Wake");
    calibrateRobot = new QPushButton("Calibrate");
    idleRobotActuator = new QPushButton("Actuator->Idle");
    actuateRobotActuator = new QPushButton("Actuator->Actuate");
    releaseRobotActuator = new QPushButton("Actuator->Release");
    demo = new QPushButton("Perform Demo");

    connect(sleepRobot, &QPushButton::clicked, this, &HomeView::sleepRobotClicked);
    connect(wakeRobot, &QPushButton::clicked, this, &HomeView::wakeRobotClicked);
    connect(calibrateRobot, &QPushButton::clicked, this, &HomeView::calibrateRobotClicked);
    connect(idleRobotActuator, &QPushButton::clicked, this, &HomeView::idleRobotActuatorClicked);
    connect(actuateRobotActuator, &QPushButton::clicked, this, &HomeView::actuateRobotActuatorClicked);
    connect(releaseRobotActuator, &QPushButton::clicked, this, &HomeView::releaseRobotActuatorClicked);
    connect(demo, &QPushButton::clicked, this, &HomeView::performDemo);

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

    connect(setRobotPosition, &QPushButton::clicked, this, &HomeView::setRobotPositionClicked);

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
    baseLayout->addLayout(hardwareLayout);
    baseLayout->addStretch();

    // Add base layout to view
    setLayout(baseLayout);

    // Initialize camera
    camera = new cv::VideoCapture(0);
    if (!camera->isOpened()) 
        std::cerr << "ERROR: No camera found" << std::endl;

    // Initialize camera feed timer
    cameraFeedTimer = new QTimer(this);
    connect(cameraFeedTimer, &QTimer::timeout, this, &HomeView::updateCameraFeed);
    cameraFeedTimer->start(20); // Update feed every 20ms
}

HomeView::~HomeView()
{
    delete camera;
    delete robot;
}

void HomeView::refreshAvailablePorts()
{
    // Clear existing ports in list
    portList->clear();
    portInfoMap->clear();

    // Populate list with name and description of all available ports
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    for (int i = 0; i < serialPorts.length(); i++)
    {
        QSerialPortInfo port = serialPorts[i];
        QString description = port.portName() + ": " + port.description();
        new QListWidgetItem(description, portList);
        portInfoMap->insert(description, port);
    }

    // Update home screen state
    connectButton->setEnabled(false);
}

void HomeView::updateCameraFeed()
{
    // Capture frame from camera
    cv::Mat frame;
    *camera >> frame;

    // Display image in camera feed
    cvtColor(frame, frame, cv::COLOR_BGR2RGB); // Convert from BGR to RGB
    QImage cameraFeedImage = QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    cameraFeed->setPixmap(QPixmap::fromImage(cameraFeedImage));
}

void HomeView::connectToRobot()
{
    // Get selected serial port
    QString selectedPort = portList->currentItem()->text();
    QSerialPortInfo selectedPortInfo = portInfoMap->value(selectedPort);
    port = new QSerialPort(selectedPortInfo, this);

    // Initialize port
    port->setBaudRate(QSerialPort::Baud115200);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    // Open port
    if (port->open(QIODevice::ReadWrite))
    {
        // TODO: Verify connection with robot
        // Update home screen state if connection successful
        baseLayout->addLayout(robotControlLayout);
        baseLayout->addSpacing(20);
        baseLayout->addLayout(robotPositionLayout);
        baseLayout->addStretch();
        portList->setEnabled(false);
        refreshButton->setEnabled(false);
        robotLayout->replaceWidget(connectButton, disconnectButton);
        delete connectButton;
        
        // Initialize robot interface
        robot = new Robot(port);
    }
}

void HomeView::portListSelectionChange()
{
    connectButton->setEnabled(portList->currentItem() != Q_NULLPTR);
}

void HomeView::sleepRobotClicked()
{
    robot->sleep();
}

void HomeView::wakeRobotClicked()
{
    robot->wake();
}

void HomeView::calibrateRobotClicked()
{
    robot->calibrate();
}

void HomeView::setRobotPositionClicked()
{
    robot->setPosition(xPosition->value(), yPosition->value(), zPosition->value(), rPosition->value());
}

void HomeView::idleRobotActuatorClicked()
{
    robot->resetGripper();
}

void HomeView::actuateRobotActuatorClicked()
{
    robot->actuateGripper();
}

void HomeView::releaseRobotActuatorClicked()
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

void HomeView::performDemo()
{
    int xSrc[numCubes] = { 1000, 937, 873, 810, 747, 683, 620, 1000, 937, 873, 810, 747, 683, 620, 1000, 937, 873 };
    int ySrc[numCubes] = { 1110, 1110, 1110, 1109, 1109, 1109, 1109, 1046, 1046, 1046, 1045, 1045, 1045, 1045, 983, 983, 983 };
    int zSrc[numCubes] = { 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180 };

    int xDest[numCubes] = {500, 500 + centreSpacing, 500 + 2 * centreSpacing, 500, 500 + centreSpacing, 500 + 2 *centreSpacing, 
        500, 500 + centreSpacing, 500 + 2 * centreSpacing, 500 + centreSpacing / 2, 500 + 3 * centreSpacing / 2, 500 + centreSpacing / 2, 
        500 + 3 * centreSpacing / 2, 500 + centreSpacing, 500 + centreSpacing, 500 + centreSpacing, 500 + centreSpacing };
    int yDest[numCubes] = {500, 500, 500, 500 + centreSpacing, 500 + centreSpacing, 500 + centreSpacing, 500 + 2 * centreSpacing, 
        500 + 2 * centreSpacing, 500 + 2 * centreSpacing, 500 + centreSpacing / 2, 500 + centreSpacing / 2, 500 + 3 * centreSpacing / 2,
        500 + 3 * centreSpacing / 2, 500 + centreSpacing, 500 + centreSpacing, 500 + centreSpacing, 500 + centreSpacing };
    int zDest[numCubes] = {baseLayer, baseLayer, baseLayer, baseLayer, baseLayer, baseLayer, baseLayer, baseLayer, baseLayer,
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