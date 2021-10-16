#pragma once

#include "Robot.h"
#include "Logger.h"
#include "OpenGLView.h"
#include "CubeWorldModel.h"
#include "CubeTask.h"
#include "opencv2/opencv.hpp"
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QTimer>

class ConstructionView : public QWidget
{
    Q_OBJECT

public:
    /*!
    * Class constructor.
    * @param [in] parent Parent widget
    */
    ConstructionView(QWidget* parent = Q_NULLPTR);

    /*!
    * Update the construction view state when it is displayed on screen.
    */
    void showView();

    /*!
    * Update the construction view state when it is hidden off screen to stop performing unnessary computations.
    */
    void hideView();

    /*!
    * Set the construction view's reference to the system robot instance.
    */
    void setRobot(Robot* robot);

    /*!
    * Set the construction view's reference to the system camera instance.
    */
    void setCamera(cv::VideoCapture* camera);

signals:
    /*!
    * Generated when a message is logged by an \class ConstructionView instance.
    */
    void log(Message message) const;

private:
    QVBoxLayout* baseLayout; /* Layout for all the main construction view components and layouts */
    QHBoxLayout* visualLayout; /* Layout for camera and shape view layouts */
    QVBoxLayout* cameraLayout; /* Layout for camera feed and supplementary widgets */
    QVBoxLayout* shapeLayout; /* Layout for 3D shape view and supplementary widgets */
    QLabel* cameraFeed; /*! Display live images captured by camera */

    QPushButton* loadModel; /* Load model to be constructed into the cube world model from JSON file */
    QPushButton* execute; /* Initiate construction of cube world model */

    OpenGLView* shapeView; /*! OpenGL render of 3D shape to be constructed */
    CubeWorldModel* cubeWorldModel; /*! Model of cubes in world frame */
    QTimer* cameraFeedTimer; /*! Timebase to refresh camera feed display */
    QTimer* openGLTimer; /*! Timer to trigger update of OpenGL shape view */
    cv::VideoCapture* camera = Q_NULLPTR; /*! Reference to source of live camera images */
    Robot* robot = Q_NULLPTR; /*! Reference to interface with the robotic subsystem */

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
    * Captures new image from camera and updates the camera feed.
    */
    void updateCameraFeed();

    /*!
    * Request OpenGL redraw the shape view.
    */
    void updateShapeView();

    /*!
    * Load cube world model from file.
    */
    void loadModelClicked();

    /*!
    * Control robot to construct cube world model.
    */
    void executeConstruction();

    void sleepRobotClicked();
    void wakeRobotClicked();
    void calibrateRobotClicked();
    void setRobotPositionClicked();
    void idleRobotActuatorClicked();
    void actuateRobotActuatorClicked();
    void releaseRobotActuatorClicked();
    void performDemo();
};
