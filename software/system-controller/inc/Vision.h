#pragma once

#include <QObject>
#include "opencv2/opencv.hpp"

class Vision : public QObject
{
	Q_OBJECT
public:
	/*!
	* Class constructor.
	* @param [in] parent Parent object.
	*/
	Vision(QObject* parent = Q_NULLPTR);

	/*!
	* Compute the corresponding world point given an image point and the Z coordinate of the world point.
	* \param [in] imagePoint Coordinates of the source point in the uv image frame.
	* \param [in] z Z coordinate of the correspoding world point.
	* \return World point with specified z coordinate corresponding to given image point.
	*/
	cv::Point3d projectImagePoint(const cv::Point2d& imagePoint, double z) const;

	/*!
	* Compute the corresponding image point given a world point.
	* \param [in] worldPoint Coordinates of the source point in the XYZ world frame.
	* \return Image point corresponding to given world point.
	*/
	cv::Point2d projectWorldPoint(cv::Point3d worldPoint) const;

private:
	double fx = 696.2920653066839 * 2; /*! Camera x-axis focal length */
	double fy = 696.1538823160478 * 2; /*! Camera y-axis focal length */
	double cx = 469.7644569362635 * 2; /*! Camera principal point x-coordinate */
	double cy = 281.0969237061734 * 2; /*! Camera principal point y-coordinate */

	cv::Mat cameraMatrix; /*! Intrinsic camera matrix */
	cv::Mat distCoeffs; /*! Camera distorition coefficients */
	cv::Mat rotationMatrix; /*! Rotation matrix for world frame with respect to camera frame */
	cv::Mat translationVector; /*! Translation matrix for world frame with respect to camera frame */
};
