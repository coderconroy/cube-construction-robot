#pragma once

#include <QWidget>
#include <QStackedLayout>
#include "HomeView.h"
#include "DesignView.h"
#include "ConstructionView.h"
#include "Logger.h"


class SystemController: public QWidget
{
    Q_OBJECT

public:
    /*!
    * Class constructor. Initializes global program variables and views.
    */
    SystemController(QWidget *parent = Q_NULLPTR);

    /*!
    * Class destructor.
    */
    ~SystemController();

private:
    QHBoxLayout* navigationLayout; /*! Layout for software navigation components */
    QVBoxLayout* baseLayout; /* Base layout for the program views and navigation components */
    QStackedLayout* viewLayout; /* Layout container for the current primary view */

    HomeView* homeView; /*! Initial view to establish hardware connections */
    DesignView* designView; /*! 3D shape definition view */
    ConstructionView* constructionView; /*! View to initiate and monitor the construction of the 3D shape */
    QPushButton* homeViewLink; /*! Navigation link for home view */
    QPushButton* designViewLink; /*! Navigation link for design view */
    QPushButton* constructionViewLink; /*! Navigation link for construction view */
    QPushButton* showMessageLog; /*! Show message log button */

    Logger* messageLog; /*! Display for all messages logged by various software components */
    Robot* robot = Q_NULLPTR; /*! Reference to interface with the robotic subsystem */
    cv::VideoCapture* camera = Q_NULLPTR; /*! Source of live camera images */

    const double CAMERA_WIDTH = 1280; /*! Robot vision camera input image width in pixels */
    const double CAMERA_HEIGHT = 720; /*! Robot vision camera input image height in pixels */
    const double CAMERA_EXPOSURE = -8; /*! Robot vision camera exposure level */
    const double CAMERA_FOCUS = 5; /*! Robot vision camera focus distance (must be a multiple of 5) */

    /*!
    * Set the current primary view based on which link button was clicked.
    */
    void setView();

    /*!
    * Hide the message log widget.
    */
    void hideMessageLog();

    /*!
    * Show the message log widget.
    */
    void showMessageLogClick();

    /*!
    * Update system state when robot connection is established
    */
    void robotConnected();
};
