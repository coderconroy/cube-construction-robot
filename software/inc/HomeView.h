#pragma once

#include "Robot.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QListWidget>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QSpinBox>
#include <QMap>
#include "opencv2/opencv.hpp"

/*!
* The \class HomeView class defines the layout and operation of the home screen. 
* The home screen is the initial screen presented to the user when the software is started.
* The purpose of the home screen is to ensure that the required camera hardware as well as 
* the robotic system are present and connected. Furthermore, it also acts as a branching point
* to access the other components of the system.. This includes the shape specification, construction
* and qualification testing components.
*/

class HomeView: public QWidget
{
    Q_OBJECT

public:
    /*!
    * Class constructor. Initializes the home screen.
    * @param [in] parent Parent widget
    */
    HomeView(QWidget* parent = Q_NULLPTR);

    /*!
    * Class destructor.
    */
    ~HomeView();
private:
    QVBoxLayout* baseLayout; /*! Base layout for the home screen */
    QVBoxLayout* cameraLayout; /* Layout for camera connection widgets */
    QVBoxLayout* robotLayout; /* Layout for robotic connection widgets */
    QHBoxLayout* hardwareLayout; /*! Layout for hardware connection widgets */
    QPushButton* refreshButton; /*! Refresh the available ports list */
    QPushButton* connectButton; /*! Trigger robot connection attempt */
    QPushButton* disconnectButton; /*! Disconnect from robot */
    QLabel* cameraFeed; /*! Display live images captured by camera */
    QLabel* portListLabel; /*! Label for available serial ports */
    QLabel* cameraHeading; /*! Heading for camera connection seciton */
    QLabel* robotHeading; /*! Heading for robot connection section */
    QListWidget* portList; /*! List of available serial ports */
    QTimer* cameraFeedTimer; /*! Timebase to refresh camera feed display */
    QMap<QString, QSerialPortInfo>* portInfoMap; /*! Map of items in serial port list to serial ports*/
    QSerialPort* port; /*! Serial port for UART communication with robot */
    cv::VideoCapture* camera = Q_NULLPTR; /*! Source of live camera images */
    Robot* robot = Q_NULLPTR; /*! Interface with the robotic subsystem */

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

    /*!
    * Update list of serial ports available.
    */
    void refreshAvailablePorts();

    /*!
    * Captures new image from camera and updates the camera feed.
    */
    void updateCameraFeed();

    /*!
    * Attempt to connect the robotic subsystem
    */
    void connectToRobot();

    /*!
    * Update home screen state when the serial port selection changes
    */
    void portListSelectionChange();

    void sleepRobotClicked();
    void wakeRobotClicked();
    void calibrateRobotClicked();
    void setRobotPositionClicked();
    void idleRobotActuatorClicked();
    void actuateRobotActuatorClicked();
    void releaseRobotActuatorClicked();
    void performDemo();
};
