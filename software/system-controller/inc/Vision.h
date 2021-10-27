#pragma once

#include <QObject>
#include "opencv2/opencv.hpp"
#include "Logger.h"
/*!
* State-based computer vision interface for the robot
*/
class Vision : public QObject
{
	Q_OBJECT
public:
	/*!
	* Class constructor.
	* 
	* @param [in] parent Parent object.
	*/
	Vision(QObject* parent = Q_NULLPTR);

	/*!
	* Process the scene for cube and fiducial detection. The extrinsic camera parameters are recomputed if the calibration
	* argument is true. The non-fiducial contours are classified as originating from the source cubes or the cubes in the
	* 3D shape based on the known centroid location of these cubes in the world frame. Contours that are not classified are
	* considered to be independent cubes. If the source cube centroids and the structure cube centroids are not provided,
	* all non-fiducial contours are assumed to be independent cubes.
	* 
	* \param [in] image Image to be processed.
	* \param [in] calibrate Recompute the extrinsic rotation and translation matrices if true.
	* \param [in] sourceCentroids Centroid coordinates of the source cubes in the world frame.
	* \param [in] structCentroids Centroid coordinates of the cubes in the structure in the world frame;
	*/
	void processScene(const cv::Mat& image, bool calibrate, std::vector<cv::Point3i>* sourceCentroids = Q_NULLPTR,
		std::vector<cv::Point3i>* structCentroids = Q_NULLPTR);

	/*!
	* Annotate image with fiducial information.
	* 
	* \param [in] image Image to add fiducial information to.
	*/
	void plotFiducialInfo(cv::Mat& image);

	/*!
	* Annotate image with independent cube information.
	* 
	* \param [in] image Image to add independent cube information to.
	*/
	void plotCubeInfo(cv::Mat& image);

	/*!
	* Annotate image with source cube information.
	*
	* \param [in] image Image to add source cube information to.
	*/
	void plotSourceCubeInfo(cv::Mat& image);

	/*!
	* Annotate image with structure cube information.
	*
	* \param [in] image Image to add structure cube information to.
	*/
	void plotStructCubeInfo(cv::Mat& image);

	/*!
	* Annotate image with the computer vision bounding box.
	* 
	* \param [in] image Image to add bounding box to.
	*/
	void plotBoundingBox(cv::Mat& image);

	/*!
	* Compute the corresponding world point given an image point and the Z coordinate of the world point.
	* 
	* \param [in] imagePoint Coordinates of the source point in the uv image frame.
	* \param [in] z Z coordinate of the correspoding world point.
	* \return World point with specified z coordinate corresponding to given image point.
	*/
	cv::Point3i projectImagePoint(const cv::Point2d& imagePoint, double z) const;

	/*!
	* Compute the corresponding image point given a world point.
	* 
	* \param [in] worldPoint Coordinates of the source point in the XYZ world frame.
	* \return Image point corresponding to given world point.
	*/
	cv::Point projectWorldPoint(const cv::Point3d& worldPoint) const;

	/*!
	* Getter for image after the grayscale and blur stage of processing.
	* 
	* \return Image after grayscale conversion and blur.
	*/
	cv::Mat getBlurredImage();

	/*!
	* Getter for image after the thresholding stage of processing.
	* 
	* \return Image after thresholding.
	*/
	cv::Mat getThresholdedImage();

	/*!
	* Getter for image after the contour detection stage of processing.
	* 
	* \return Image after contour detection with contours plotted.
	*/
	cv::Mat getContourImage();

	/*!
	* Getter for the isolated fiducial images.
	* 
	* \return List of isolated fiducial images.
	*/
	std::vector<cv::Mat> getFiducialImages();

signals:
	/*!
	* Generated when a message is logged by an \class Vision instance.
	*/
	void log(Message message) const;

private:
	/*!
	* Collection of properties associated with a fiducial contour in the image frame.
	*/
	struct FiducialContour
	{
		int id; /*! Unique fiducial identifier as encoded in fiducial pattern */
		cv::Point centroid; /*! Centroid moment of fiducial contour */
		std::vector<cv::Point> contour; /*! Collection of points describing contour around fiducial */
		std::vector<cv::Point> corners; /*! Set of four corners of the fiducial square in an anti-clockwise direction */
		cv::Mat homographyMatrix; /*! Homography matrix mapping fiducials from calibration image to isolated image */
	};

	/*!
	* Collection of properties associated with a cube contour in the image frame.
	*/
	struct CubeContour
	{
		cv::Point centroid; /*! Centroid moment of cube contour */
		std::vector<cv::Point> contour; /*! Collection of points describing contour around cube */
		std::vector<cv::Point> corners; /*! Set of four corners of the cube top-face in an anti-clockwise direction */
	};

	// Intrinsic camera parameters
	double fx = 696.2920653066839 * 2; /*! Camera x-axis focal length */
	double fy = 696.1538823160478 * 2; /*! Camera y-axis focal length */
	double cx = 469.7644569362635 * 2; /*! Camera principal point x-coordinate */
	double cy = 281.0969237061734 * 2; /*! Camera principal point y-coordinate */

	cv::Mat cameraMatrix; /*! Intrinsic camera matrix */
	cv::Mat distCoeffs; /*! Camera distorition coefficients */
	cv::Mat rotationMatrix; /*! Rotation matrix for world frame with respect to camera frame */
	cv::Mat translationVector; /*! Translation matrix for world frame with respect to camera frame */
	std::vector<FiducialContour> fiducialContours; /*! Set of fiducials identified in the image frame */
	std::vector<CubeContour> cubeContours;  /*! Set of independent cube contours in the image frame */
	std::vector<CubeContour> sourceCubeContours;  /*! Set of source cube contours in the image frame */
	std::vector<CubeContour> structCubeContours;  /*! Set of structure cube contours in the image frame */
	QMap<int, cv::Point3i> fiducialWorldPoints; /*! Position in the world frame of each fiducial used */
	bool calibrated = false; /*! Flag to indicate if the vision system has been calibrated with a valid extrinsic matrix */
	cv::Point3i boundingBoxCorners[4]; /*! Coordinates of bounding box corners for computer vision system in the world frame */

	cv::Mat blurredImage; /*! image after the grayscale and blur stage of processing */
	cv::Mat thresholdImage; /*! Image after the thresholding stage of processing */
	cv::Mat contourImage; /*! Image after the contour detection stage of processing */
	std::vector<cv::Mat> fiducialImages; /*! Isolated fiducial images */

	/*!
	* Get the contour centroid.
	* 
	* \param [in] contour Contour to find centroid from.
	* \return Centroid point of contour.
	*/
	cv::Point getCentroid(const std::vector<cv::Point>& contour) const;

	/*!
	* Assumes the input contour is a square and estimates the square corners.
	* 
	* \param [in] contour Contour to estimate corners from.
	* \return Estimated coordinates for 4 corners running anti-clockwise around the square.
	*/
	std::vector<cv::Point> findSquareCorners(const std::vector<cv::Point>& contour) const;

	/*!
	* Isolate rectangle describes by corners in image space and warp to image.
	* 
	* \param [in] corners Four corners of rectangle in image space.
	* \param [in] sourceImage Image in which the rectangle is located.
	* \param [out] isolatedImage Image to which the rectangle is warped.
	* \param [out] homographyMatrix Homography matrix used to warp perspective from source image to isolated image.
	*/
	void isolateRectangle(const std::vector<cv::Point>& corners, const cv::Mat& sourceImage, cv::Mat& isolatedImage, cv::Mat& homographyMatrix) const;

	/*!
	* Find the identifier of an isolated fiducial image.
	* 
	* \param inputImage Input fiducial image.
	* \param outputImage Correctly oriented fiducial image.
	* \return Fiducial identifer. Returns -1 if not a valid fiducial.
	*/
	int processFiducial(const cv::Mat& inputImage, cv::Mat& outputImage) const;

	/*!
	* Map angle to the range (-PI, PI] randians.
	* 
	* \param [in] Angle to map in radians.
	* \return Mapped angle in radians.
	*/
	float mapAngle(float angle) const;

	/*!
	* Map angle to the range (-PI / 4, PI / 4] to describe cube rotation.
	* 
	* \param [in] Angle to map in radians.
	* \return Mapped angle in radians.
	*/
	float mapToCubeAngle(float angle) const;

	/*!
	* Compute the angle of rotation about the z-axis based on the corners of the cube contour.
	* 
	* \param [in] corners Corners of the cube contour with respect to the image frame in an anti-clockwise direction.
	* \param [in] z Z coordinate of the xy plane to estimate the orientation of the cube in.
	* \param [in] centroid Centroid of the top cube face contour with respect to the image frame.
	* \return Estimated rotation of cube about z-axis in radians.
	*/
	float computeCubeZRotation(const std::vector<cv::Point>& corners, const cv::Point centroid, const int z) const;

	/*!
	* Compute the Euclidean distance between two points in 3D space.
	* \param [in] pointA Coordinates of the first coordinate.
	* \param [in] pointB Coordinates of the second coordinate.
	*/
	double computeEuclidDist(const cv::Point3i& pointA, const cv::Point3i& pointB);
};
