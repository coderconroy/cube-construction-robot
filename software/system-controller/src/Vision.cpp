#include "Vision.h"

Vision::Vision(QObject* parent) : QObject(parent)
{
	// Initialize world points
	std::vector<cv::Point3d> worldPoints;
	worldPoints.push_back(cv::Point3d(545, 830, -64));
	worldPoints.push_back(cv::Point3d(543, 519, -64));
	worldPoints.push_back(cv::Point3d(540, 206, -64));
	worldPoints.push_back(cv::Point3d(889, 827, -64));
	worldPoints.push_back(cv::Point3d(888, 518, -64));
	worldPoints.push_back(cv::Point3d(886, 207, -64));
	worldPoints.push_back(cv::Point3d(1233, 826, -64));
	worldPoints.push_back(cv::Point3d(1232, 515, -64));
	worldPoints.push_back(cv::Point3d(1232, 204, -64));

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
	cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
	cv::Mat distCoeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type);

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
	return cv::Point2d();
}