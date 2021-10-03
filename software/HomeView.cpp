#include "HomeView.h"
#include "iostream"
#include <QSerialPortInfo>
#include <QList>

HomeView::HomeView(QWidget* parent) : QWidget(parent)
{
    // Initialize camera feed
    cameraFeed = new QLabel();

    // Initialize available serial ports list
    portListLabel = new QLabel(tr("Available Ports:"));
    portList = new QListWidget();

    // Populate available serial ports
    refreshAvailablePorts();

    // Initialize serial port layout
    portLayout = new QVBoxLayout();
    portLayout->addWidget(portListLabel);
    portLayout->addWidget(portList);

    // Initialize hardware layout
    hardwareLayout = new QHBoxLayout();
    hardwareLayout->addWidget(cameraFeed);
    hardwareLayout->addLayout(portLayout);

    // Initialize base layout
    baseLayout = new QVBoxLayout();
    baseLayout->addLayout(hardwareLayout);

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
    portList->clear();
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    for (int i = 0; i < serialPorts.length(); i++)
    {
        QSerialPortInfo port = serialPorts[i];
        QString description = port.portName() + ": " + port.description();
        new QListWidgetItem(description, portList);
    }
}

void HomeView::updateCameraFeed()
{
    // Capture frame from camera
    cv::Mat frame;
    *camera >> frame;

    // Display image in camera feed
    cvtColor(frame, frame, cv::COLOR_BGR2RGB); // COnvert from BGR to RGB
    QImage cameraFeedImage = QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    cameraFeed->setPixmap(QPixmap::fromImage(cameraFeedImage));
}
