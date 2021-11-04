#pragma once

#include "Robot.h"
#include "Cube.h"
#include "Logger.h"
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
	* Set the cube in the world frame for which the cube task is to be performed.
	* 
	* \param [in] sourceCube Cube in the source position for which the cube task is performed.
	*/
	void setSourceCube(Cube* sourceCube);

	/*!
	* Get the cube in the destination position of the source cube as defined in the build model.
	*
	* \param [in] sourceCube Cube in the build model destination position.
	*/
	void setDestinationCube(Cube* destinationCube);

	/*!
	* Get the cube in the world frame for which the cube task is to be performed.
	*
	* \return Cube in the source position for which the cube task is performed.
	*/
	Cube* getSourceCube();

	/*!
	* Get the cube in the destination position of the source cube as defined in the build model.
	*
	* \return Cube in the build model destination position.
	*/
	Cube* getDestinationCube();

	/*!
	* Indicates if there are any steps remaining to be performed for the cube task.
	* \return True if the cube task is complete. False otherwise.
	*/
	bool isComplete();

	/*!
	* Executes the next step in the cube task.
	* 
	* \param [out] robot Robot used to execute next cube task step.
	*/
	void performNextStep(Robot* robot);

	/*!
	* Indicates if the instruction to complete the first step in the task has been issued.
	* 
	* return True if the instruction to compelte the first step has been issued. False otherwise.
	*/
	bool isStarted();

	/*!
	* Indicates if the cube task expects the cube to be gripped for its current step.
	* 
	* \return True if the current step expects the cube to be gripped. False otherwise.
	*/
	bool expectGrippedCube();

	/*!
	* Reset the cube step counter to the first step.
	*/
	void resetSteps(Robot* robot);

signals:
	/*!
	* Generated when a message is logged by a \class CubeTask instance.
	*/
	void log(Message message) const;

private:
	Cube* destinationCube = Q_NULLPTR; /*! Reference cube in the destination position of the source cube as defined in the build model */
	Cube* sourceCube = Q_NULLPTR; /*! Cube in the source position for which the cube task is performed */
	int cubeLengthHSteps = 64; /*! Length of the cube edge in horizontal steps */
	int cubeLengthVSteps = 318; /*! Length of the cube edge in vertical steps */
	int bufferAction = 150; /*! Number of vertical steps the robot moves below the target z-axis position to ensure a reliable connection */

	// TODO: Review perform step implementation
	int step = 0;
	bool taskComplete = false;
	int xOffset = 600;
	int yOffset = 800;
};

