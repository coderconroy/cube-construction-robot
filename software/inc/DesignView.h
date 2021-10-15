#pragma once

#include "OpenGLView.h"
#include "CubeWorldModel.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <QListWidget>

class DesignView: public QWidget
{
    Q_OBJECT
public:
    /*!
    * Class constructor. Initializes the home screen.
    * @param [in] parent Parent widget
    */
    DesignView(QWidget* parent = Q_NULLPTR);

    /*!
    * Update the design view state when it is displayed on screen.
    */
    void showView();

    /*!
    * Update the design view state when it is hidden off screen to stop performing unnessary computations.
    */
    void hideView();

signals:
    /*!
    * Generated when a message is logged by an \class DesignView instance.
    */
    void log(Message message) const;

private:
    QHBoxLayout* baseLayout; /*! Layout to store the fundamental design view widgets */
    QVBoxLayout* controlsLayout; /*! Layout for the 3D shape design controls */
    OpenGLView* shapeView; /*! OpenGL render of 3D shape to be constructed */
    CubeWorldModel* cubeWorldModel; /*! Model of cubes in world frame */
    QListWidget* cubeList; /*! List of all cubes that have been added to the cube world */
    QPushButton* insertCube; /*! Add cube to the cube world model */
    QPushButton* removeCube; /*! Remove cube from the cube world model */
    QPushButton* saveModel; /*! Save cube world model to file */
    QPushButton* loadModel; /*! Load cube world model from file */
    QPushButton* clearModel; /*! Remove all cubes from the cube world model */
    QMap<QString, const Cube*> cubeMap; /*! Map of items in cube list to cube instances */
    QTimer* openGLTimer; /*! Timer to trigger update of OpenGL shape view */

    /*!
    * Request OpenGL redraw the shape view.
    */
    void updateShapeView();

    /*! 
    * Add cube to the cube world model.
    */
    void insertCubeClicked();

    /*!
    * Remove cube from the cube world model.
    */
    void removeCubeClicked();

    /*!
    * Save cube world model to file.
    */
    void saveModelClicked();

    /*!
    * Load cube world model from file.
    */
    void loadModelClicked();

    /*!
    * Remove all cubes from the cube world model.
    */
    void clearModelClicked();

    /*!
    * Update home screen state when the cube list selection changes
    */
    void cubeListSelectionChange();

    /*!
    * Update state of the 3D shape design controls to coincide with the cube world model state.
    */
    void updateControlState();
};