#include "Vision.h"
#include <iostream>

Vision::Vision(QObject* parent) : QObject(parent)
{
	// Initialize world points
	std::vector<cv::Point3d> worldPoints;
	worldPoints.push_back(cv::Point3d(0, 0, -64));
	worldPoints.push_back(cv::Point3d(507, 0, -64));
	worldPoints.push_back(cv::Point3d(1015, 0, -64));
	worldPoints.push_back(cv::Point3d(0, 562, -64));
	worldPoints.push_back(cv::Point3d(507, 562, -64));
	worldPoints.push_back(cv::Point3d(1015, 562, -64));
	worldPoints.push_back(cv::Point3d(0, 1125, -64));
	worldPoints.push_back(cv::Point3d(507, 1125, -64));
	worldPoints.push_back(cv::Point3d(1015, 1125, -64));

	// Initialize image points
	std::vector<cv::Point2d> imagePoints;
	imagePoints.push_back(cv::Point2d(545, 830));
	imagePoints.push_back(cv::Point2d(543, 519));
	imagePoints.push_back(cv::Point2d(540, 206));
	imagePoints.push_back(cv::Point2d(889, 827));
	imagePoints.push_back(cv::Point2d(888, 518));
	imagePoints.push_back(cv::Point2d(886, 207));
	imagePoints.push_back(cv::Point2d(1233, 826));
	imagePoints.push_back(cv::Point2d(1232, 515));
	imagePoints.push_back(cv::Point2d(1232, 204));

	// Initialize camera matrix and distortion coefficients
	cameraMatrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
	distCoeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type);

	// Compute pose of world frame with respect to the fixed camera frame
	cv::Mat rotationVector;
	cv::solvePnP(worldPoints, imagePoints, cameraMatrix, distCoeffs, rotationVector, translationVector);

	// Convert rotation vector to rotation matrix
	cv::Rodrigues(rotationVector, rotationMatrix);
}

cv::Point3d Vision::projectImagePoint(const cv::Point2d& imagePoint, double z) const
{

	return cv::Point3d();
}

cv::Point2d Vision::projectWorldPoint(cv::Point3d worldPoint) const
{
	// Compute homogenous image point
	cv::Mat imagePoint = cameraMatrix * (rotationMatrix * cv::Mat(worldPoint, false) + translationVector);

	// Normalize and return uv frame image point
	double s = imagePoint.at<double>(2);
	return cv::Point2d(imagePoint.at<double>(0) / s, imagePoint.at<double>(1) / s);
}