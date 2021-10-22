#pragma once

#include "Robot.h"
#include "Logger.h"
#include "OpenGLView.h"
#include "CubeWorldModel.h"
#include "CubeTask.h"
#include "opencv2/opencv.hpp"
#include "Vision.h"
#include <QWidget>
#include <qstackedlayout.h>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QTimer>
#include <QList>
#include <QRadioButton>

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
    QStackedLayout* baseLayout; /*! Layout containing the overview, camera and model layouts*/

    // Overview layout widgets
    QVBoxLayout* overviewLayout; /*! Layout for all the main construction view components and layouts */
    QHBoxLayout* visualLayout; /*! Layout for camera and shape view layouts */
    QVBoxLayout* overviewCameraLayout; /*! Layout for camera feed and supplementary widgets on the overview page */
    QVBoxLayout* overviewModelLayout; /*! Layout for 3D model view and supplementary widgets on the overview page */
    QLabel* overviewCameraFeed; /*! Display live images captured by camera */

    QPushButton* loadModel; /* Load model to be constructed into the cube world model from JSON file */
    QPushButton* execute; /* Initiate construction of cube world model */
    QList<CubeTask*> cubeTasks; /*! List of cube tasks to be completed for the current construction task */

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
    QLabel* xPositionLabel;
    QLabel* yPositionLabel;
    QLabel* zPositionLabel;
    QLabel* rPositionLabel;
    QSpinBox* xPosition;
    QSpinBox* yPosition;
    QSpinBox* zPosition;
    QSpinBox* rPosition;

    // Vision layout widgets
    QHBoxLayout* visionLayout; /*! Layout showing the computer vision system process and output */
    QVBoxLayout* visionControls; /*! Layout for the control widgets used to configure the computer vision display */
    QStackedLayout* visionVisual; /*! Layout for the computer vision visual display component */
    QButtonGroup* visionStageGroup; /*! Radio button group for computer vision stage selection radio buttons */
    QRadioButton* visionInput; /*! Display computer vision image raw camera input */
    QRadioButton* visionBlurred; /*! Display computer vision image after blur and grayscale stages */
    QRadioButton* visionThreshold; /*! Display computer vision image after threshold stage */
    QRadioButton* visionContours; /*! Display computer vision image after contour detection stage */
    QRadioButton* visionFiducials; /*! Display computer vision fiducial processing stage */
    QCheckBox* boundingBox; /*! Annotate plain computer vision raw image input with bounding box */
    QCheckBox* fiducialInfo; /*! Annotate plain computer vision raw image input with fiducial information */
    QCheckBox* cubeInfo; /*! Annotate plain computer vision raw image input with cube information */
    QSpinBox* worldPointXPos; /*! X coordinate of world point to project to image frame */
    QSpinBox* worldPointYPos; /*! Y coordinate of world point to project to image frame */
    QSpinBox* worldPointZPos; /*! Z coordinate of world point to project to image frame */
    QPushButton* projectWorldPoint; /*! Project specified world point to the image frame */

    // Model layout widgets
    QHBoxLayout* modelLayout; /*! Layout showing the computer vision system process and output */
    QVBoxLayout* modelControls; /*! Layout for the control widgets used to configure the computer vision display */
    QButtonGroup* modelInputGroup; /*! Radio button group for 3D model display input selection */
    QRadioButton* showBuildModel; /*! Select the model to be built as input the the 3D display */
    QRadioButton* showWorldModel; /*! Select the model of cubes in world during construction as input the the 3D display */

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

    /*!
    * Send next command to robot when previous command is complete.
    */
    void handleRobotCommand();

    void sleepRobotClicked();
    void wakeRobotClicked();
    void calibrateRobotClicked();
    void setRobotPositionClicked();
    void idleRobotActuatorClicked();
    void actuateRobotActuatorClicked();
    void releaseRobotActuatorClicked();
};
