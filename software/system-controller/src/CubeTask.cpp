#include "CubeTask.h"

CubeTask::CubeTask(QObject* parent): QObject(parent) {}

void CubeTask::setSourceCube(Cube* sourceCube)
{
	this->sourceCube = sourceCube;
}

void CubeTask::setDestinationCube(Cube* destinationCube)
{
	this->destinationCube = destinationCube;
}

Cube* CubeTask::getSourceCube()
{
	return sourceCube;
}

Cube* CubeTask::getDestinationCube()
{
	return destinationCube;
}

bool CubeTask::isComplete()
{
	return taskComplete;
}

void CubeTask::performNextStep(Robot* robot)
{
	// Verify that the source cube and destination cube positions have been initialized
	if (sourceCube == Q_NULLPTR)
	{
		emit log(Message(MessageType::ERROR_LOG, "Cube Task", "Cannot perform cube task step since no source cube has been specified"));
		return;
	}
	else if (destinationCube == Q_NULLPTR)
	{
		emit log(Message(MessageType::ERROR_LOG, "Cube Task", "Cannot perform cube task step since no destination cube has been specified"));
		return;
	}

	// Initialize the position where the cube is picked up from (x, y, z)
	// The OpenGL coordinates of the cube are converted to the robot coordinate system here
	glm::vec3 sourceCubePos = sourceCube->getPosition();
	int srcRot = round(glm::degrees(sourceCube->getPitch()) / 1.8);
	int srcPos[3] = {round(sourceCubePos.x),
		round(sourceCubePos.z), 
		round((sourceCubePos.y + 32) * cubeLengthVSteps / cubeLengthHSteps)};

	// Initialize the position where the cube is placed (x, y, z)
	// The OpenGL coordinates of the cube are converted to the robot coordinate system here
	glm::vec3 destinationCubePos = destinationCube->getPosition();
	int destRot = round(glm::degrees(destinationCube->getPitch()) / 1.8);
	int destPos[3] = { round(destinationCubePos.x) + xOffset, 
		round(destinationCubePos.z) + yOffset,
		round((destinationCubePos.y + 32) * cubeLengthVSteps / cubeLengthHSteps)};

	// Instruct robot to perform next step
	switch (++step)
	{
	// Pick up cube at source
	case 1:
		robot->setPosition(srcPos[0], srcPos[1], robot->getZPosition(), srcRot);
		break;
	case 2:
		robot->setPosition(srcPos[0], srcPos[1], srcPos[2] - bufferAction, srcRot);
		break;
	case 3:
		robot->actuateGripper();
		break;
	case 4:
		robot->delay();
		break;
	case 5:
		robot->setPosition(srcPos[0], srcPos[1], destPos[2] + cubeLengthVSteps, srcRot);
		break;
	// Place cube at destination
	case 6:
		robot->setPosition(destPos[0], destPos[1], destPos[2] + cubeLengthVSteps, destRot);
		break;
	case 7:
		robot->setPosition(destPos[0], destPos[1], destPos[2] - bufferAction, destRot);
		break;
	case 8:
		robot->releaseGripper();
		break;
	case 9:
		robot->delay();
		break;
	case 10:
		robot->setPosition(destPos[0], destPos[1], destPos[2], destRot);
		break;
	case 11:
		robot->resetGripper();
		taskComplete = true;
		break;
	}
}

void CubeTask::resetSteps(Robot* robot)
{
	step = 0;
	taskComplete = false;
	robot->resetGripper();
}

bool CubeTask::expectGrippedCube()
{
	return step == 5 || step == 6 || step == 7;
}

bool CubeTask::isStarted()
{
	return step > 0;
}