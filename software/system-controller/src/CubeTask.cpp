#include "CubeTask.h"

CubeTask::CubeTask(QObject* parent): QObject(parent)
{

}

void CubeTask::setSourcePose(int xPos, int yPos, int zPos, int zRotation)
{
	srcPos[0] = xPos;
	srcPos[1] = yPos;
	srcPos[2] = zPos;
	srcRot = zRotation;
}


void CubeTask::setDestinationPose(int xPos, int yPos, int zPos, int zRotation)
{
	destPos[0] = xPos;
	destPos[1] = yPos;
	destPos[2] = zPos;
	destRot = zRotation;
}

bool CubeTask::isComplete()
{
	return taskComplete;
}

void CubeTask::performNextStep(Robot* robot)
{
	switch (step)
	{
	// Pick up cube at source
	case 0:
		robot->setPosition(srcPos[0], srcPos[1], robot->getZPosition(), 0);
		break;
	case 1:
		robot->setPosition(srcPos[0], srcPos[1], srcPos[2] - bufferAction, 0);
		break;
	case 2:
		robot->actuateGripper();
		break;
	case 3:
		robot->delay();
		break;
	case 4:
		robot->setPosition(srcPos[0], srcPos[1], destPos[2] + moveOffset + cubeHeight, 0);
		break;
	// Place cube at destination
	case 5:
		robot->setPosition(destPos[0], destPos[1], destPos[2] + moveOffset + cubeHeight, destRot);
		break;
	case 6:
		robot->setPosition(destPos[0], destPos[1], destPos[2] - bufferAction, destRot);
		break;
	case 7:
		robot->releaseGripper();
		break;
	case 8:
		robot->delay();
		break;
	case 9:
		robot->setPosition(destPos[0], destPos[1], destPos[2] + moveOffset, destRot);
		break;
	case 10:
		robot->resetGripper();
		taskComplete = true;
		break;
	}

	step++;
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