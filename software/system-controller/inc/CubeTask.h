#pragma once

#include "Robot.h"
#include <QObject>

/*!
* A cube task is the collection of constituent robot steps that need to be performed to move a cube from 
* its source pose to its destination pose. The cube task is defined in terms of the robot coordinate system.
*/
class CubeTask : public QObject
{
	Q_OBJECT
public:
	/*!
	* Class constructor.
	* @param [in] parent Parent object
	*/
	CubeTask(QObject* parent = Q_NULLPTR);

	/*!
	* Set the initial pose of the cube. The position refers to the centre of the cube.
	* \param [in] xPos Initial cube position along x-axis in horizontal steps.
	* \param [in] yPos Initial cube position along y-axis in horizontal steps.
	* \param [in] zPos Initial cube position along z-axis in vertical steps.
	* \param [in] zRotation Initial cube rotation about z-axis in rotation steps.
	*/
	void setSourcePose(int xPos, int yPos, int zPos, int zRotation);

	/*!
	* Set the final pose of the cube. The position refers to the centre of the cube.
	* \param [in] xPos Final cube position along x-axis in horizontal steps.
	* \param [in] yPos Final cube position along y-axis in horizontal steps.
	* \param [in] zPos Final cube position along z-axis in vertical steps.
	* \param [in] zRotation Final cube rotation about z-axis in rotation steps.
	*/
	void setDestinationPose(int xPos, int yPos, int zPos, int zRotation);

	/*!
	* Indicates if there are any steps remaining to be performed for the cube task.
	* \return True if the cube task is complete. False otherwise.
	*/
	bool isComplete();

	/*!
	* Executes the next step in the cube task.
	* \param [out] robot Robot used to execute next cube task step.
	*/
	void performNextStep(Robot* robot);

	/*!
	* Reset the cube step counter to the first step.
	*/
	void resetSteps();

private:
	unsigned int cubeHeight = 318; /*! Length of the cube edge in vertical steps */
	int moveOffset = 200; /*! Number of vertical steps in clearance along z-axis during cube translation */
	int bufferAction = 50; /*! Number of vertical steps the robot moves below the target z-axis position to ensure a reliable connection */
	int srcPos[3]; /*! Position where the cube is picked up from (x, y, z) */
	int destPos[3]; /*! Destination position to place the cube (x, y, z) */
	int srcRot; /*! Rotation of cube about z-axis in its source position */
	int destRot; /*! Rotation of cube about z-axis in its destination position */

	// TODO: Review perform step implementation
	int step = 0;
	bool taskComplete = false;
};

