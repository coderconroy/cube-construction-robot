#include "HomeView.h"
#include "iostream"
#include <QSerialPortInfo>
#include <QList>

HomeView::HomeView(QWidget* parent) : QWidget(parent)
{
    // Initialize stylesheets
    QString headingStyleSheet = 
        "font-size: 30px;"
        "font-family: Calibri, sans-serif";

    // Initialize camera feed
    cameraHeading = new QLabel("Camera Connection");
    cameraHeading->setStyleSheet(headingStyleSheet);
    cameraFeed = new QLabel();

    // Initialize robotic system connection widgets
    robotHeading = new QLabel("Robot Connection");
    robotHeading->setStyleSheet(headingStyleSheet);
    portListLabel = new QLabel("Available Ports:");
    portList = new QListWidget();
    refreshButton = new QPushButton("Refresh Ports");
    connectButton = new QPushButton("Connect to Robot");
    connectButton->setEnabled(false);

    refreshAvailablePorts();

    connect(portList, &QListWidget::itemSelectionChanged, this, &HomeView::portListSelectionChange);
    connect(refreshButton, &QPushButton::clicked, this, &HomeView::refreshAvailablePorts);
    connect(connectButton, &QPushButton::clicked, this, &HomeView::connectToRobot);

    // Initialize camera connection layout
    cameraLayout = new QVBoxLayout();
    cameraLayout->addWidget(cameraHeading);
    cameraLayout->addWidget(cameraFeed);

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
}

void HomeView::refreshAvailablePorts()
{
    // Clear existing ports in list
    portList->clear();

    // Populate list with name and description of all available ports
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    for (int i = 0; i < serialPorts.length(); i++)
    {
        QSerialPortInfo port = serialPorts[i];
        QString description = port.portName() + ": " + port.description();
        new QListWidgetItem(description, portList);
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

}

void HomeView::portListSelectionChange()
{
    connectButton->setEnabled(portList->currentItem() != NULL);
}