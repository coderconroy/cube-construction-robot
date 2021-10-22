#include "Vision.h"
#include <iostream>

// Threshold parameters
int thresh = 80;
int maxThresh = 255;
int blurSize = 1;
int maxBlurSize = 20;

// Fiducials
float fiducialThresh = 0.7f;
int fiducialWidth = 128;
int fiducialHeight = 128;

// Fiducial sample points
std::vector<cv::Point> fiducialSamplePoints;

// Bounding box parameters
bool showCoords = true;
double areaThreshold = 1300;

Vision::Vision(QObject* parent) : QObject(parent)
{
    // Initialize world point of each fiducial
    fiducialWorldPoints.insert(11, cv::Point3i(0, 0, 0));
    fiducialWorldPoints.insert(37, cv::Point3i(0, 562, 0));
    fiducialWorldPoints.insert(10, cv::Point3i(0, 1125, 0));
    fiducialWorldPoints.insert(3, cv::Point3i(1015, 0, 0));
    fiducialWorldPoints.insert(6, cv::Point3i(1015, 562, 0));
    fiducialWorldPoints.insert(41, cv::Point3i(1015, 1125, 0));

    // Initialize fiducial sample points
    fiducialSamplePoints.push_back(cv::Point(31, 31));
    fiducialSamplePoints.push_back(cv::Point(63, 31));
    fiducialSamplePoints.push_back(cv::Point(95, 31));
    fiducialSamplePoints.push_back(cv::Point(31, 63));
    fiducialSamplePoints.push_back(cv::Point(63, 63));
    fiducialSamplePoints.push_back(cv::Point(95, 63));
    fiducialSamplePoints.push_back(cv::Point(31, 95));
    fiducialSamplePoints.push_back(cv::Point(63, 95));
    fiducialSamplePoints.push_back(cv::Point(95, 95));

    // Initialize camera matrix and distortion coefficients
    cameraMatrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    distCoeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type);

    // Initialize coordinates of bounding box for computer vision consideration in world coordinates
    boundingBoxCorners[0] = cv::Point3i(-100, -100, 0);
    boundingBoxCorners[1] = cv::Point3i(1115, -100, 0);
    boundingBoxCorners[2] = cv::Point3i(1115, 1225, 0);
    boundingBoxCorners[3] = cv::Point3i(-100, 1225, 0);
}

void Vision::calibrate(const cv::Mat& calibrationImage)
{
    // Reset vision system to uncalibrated state
    fiducialContours.clear();
    cubeContours.clear();
    calibrated = false;

    // Process image
    cv::Mat processImage;
    cv::cvtColor(calibrationImage, processImage, cv::COLOR_BGR2GRAY);
    cv::blur(processImage, processImage, cv::Size(blurSize + 1, blurSize + 1));
    cv::threshold(processImage, processImage, thresh, maxThresh, cv::THRESH_BINARY);

    // Apply contour detection
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(processImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Process contours for cubes and fiducials
    for (int i = 0; i < contours.size(); i++)
    {
        // Get contour
        std::vector<cv::Point> contour = contours[i];

        // Get contour centroid and area
        cv::Point2d centroid = getCentroid(contour);
        double area = cv::contourArea(contours[i]);

        // Process contours of significant size in bounding box 
        if (area > areaThreshold)
        {
            // Find corners
            std::vector<cv::Point> corners = findSquareCorners(contour);

            // Isolate rectangle
            cv::Mat isolatedImage(fiducialWidth, fiducialHeight, CV_8UC1);
            cv::Mat homographyMatrix;
            isolateRectangle(corners, processImage, isolatedImage, homographyMatrix);
            cv::threshold(isolatedImage, isolatedImage, thresh, maxThresh, cv::THRESH_BINARY);

            // Process fiducial
            int fiducialId = processFiducial(isolatedImage, isolatedImage);

            // Add to fiducial list if fiducial
            if (fiducialId >= 0) 
            {
                FiducialContour fiducial;
                fiducial.id = fiducialId;
                fiducial.centroid = centroid;
                fiducial.contour = contour;
                fiducial.corners = corners;
                fiducial.homographyMatrix = homographyMatrix;
                fiducialContours.push_back(fiducial);
            }
        }
    }

    // Get world points and corresponding image points from fiducial set
    std::vector<cv::Point3d> worldPoints;
    std::vector<cv::Point2d> imagePoints;
    for (int i = 0; i < fiducialContours.size(); ++i)
    {
        FiducialContour f = fiducialContours[i];
        
        // Check if world point is defined for fiducial
        if (fiducialWorldPoints.contains(f.id))
        {
            worldPoints.push_back(fiducialWorldPoints.value(f.id)); // Get known world point of fiducial
            imagePoints.push_back(f.centroid); // Use centroid of fiducial contour as image point
        }
    }

    // Compute pose of world frame with respect to the fixed camera frame
    if (worldPoints.size() > 4)
    {
        // Solve for pose
        cv::Mat rotationVector;
        cv::solvePnP(worldPoints, imagePoints, cameraMatrix, distCoeffs, rotationVector, translationVector);

        // Convert rotation vector to rotation matrix
        cv::Rodrigues(rotationVector, rotationMatrix);

        // Transition system to calibrated state
        calibrated = true;
    }
}

void Vision::plotFiducialInfo(cv::Mat& image)
{
    // Plot fiducial information
    for (int i = 0; i < fiducialContours.size(); ++i)
    {
        // Get fiducial
        FiducialContour f = fiducialContours[i];

        // Plot contour
        std::vector<std::vector<cv::Point>> contours = { f.contour };
        //cv::drawContours(image, contours, 0, cv::Scalar(0, 255, 0), 4);

        // Plot identifier
        cv::Point textPoint(f.centroid.x - 20, f.centroid.y + 10);
        cv::putText(image, std::to_string(f.id), textPoint, cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(0, 0, 255), 2);

        // Plot corners
        for (int j = 0; j < f.corners.size(); ++j)
            circle(image, f.corners[j], 4, cv::Scalar(0, 255, 255), -1, cv::LINE_AA);
    }

    cv::Point3i worldPoint = cv::Point3i(500, 0, -384);
    cv::Point imagePoint = projectWorldPoint(worldPoint);
    circle(image, imagePoint, 4, cv::Scalar(0, 0, 255), -1, cv::LINE_AA);
}

void Vision::plotBoundingBox(cv::Mat& image)
{
    // Project bounding box world coordinates to image coordinates
    cv::Point imageCoordinatesL[4]; // Lower bounding box
    cv::Point imageCoordinatesH[4]; // Upper bounding box
    for (int i = 0; i < 4; ++i)
    {
        imageCoordinatesL[i] = projectWorldPoint(boundingBoxCorners[i]);
        imageCoordinatesH[i] = projectWorldPoint(boundingBoxCorners[i] + cv::Point3i(0, 0, 7 * -64));
    }

    // Plot bounding box
    for (int i = 0; i < 4; ++i) 
    {
        cv::line(image, imageCoordinatesL[i], imageCoordinatesL[(i + 1) % 4], cv::Scalar(255, 255, 0), 3, cv::LINE_8);
        cv::line(image, imageCoordinatesH[i], imageCoordinatesH[(i + 1) % 4], cv::Scalar(255, 255, 0), 3, cv::LINE_8);
        cv::line(image, imageCoordinatesL[i], imageCoordinatesH[i], cv::Scalar(255, 255, 0), 3, cv::LINE_8);
    }
}

cv::Point Vision::getCentroid(const std::vector<cv::Point>& contour) const
{
    cv::Moments contourMoments = moments(contour, false);
    cv::Point centroid = cv::Point2f(contourMoments.m10 / contourMoments.m00, contourMoments.m01 / contourMoments.m00);
    return centroid;
}

std::vector<cv::Point> Vision::findSquareCorners(const std::vector<cv::Point>& contour) const
{
    // Find the contour point furthest from the centroid and take as first corner
    cv::Point centroid = getCentroid(contour);
    std::vector<cv::Point> corners(4);
    std::vector<double> distances(contour.size()); // Distance of each point from centroid
    float maxDist[4] = {0, 0, 0, 0}; // Maximum distance for each quadrant
    for (int i = 0; i < contour.size(); i++)
    {
        // Compute Euclidean distance of point to centroid
        distances[i] = sqrt(pow(contour[i].x - centroid.x, 2) + pow(contour[i].y - centroid.y, 2));

        // Check if further than furthest point found so far
        if (distances[i] > maxDist[0])
        {
            maxDist[0] = distances[i];
            corners[0] = contour[i];
        }
    }

    // Compute angle between x axis and line from centroid to furthest point and quadrant reference angles
    float refAngle1 = atan2(corners[0].y - centroid.y, corners[0].x - centroid.x);
    float refAngle2 = mapAngle(refAngle1 + M_PI / 2);
    float refAngle3 = mapAngle(refAngle1 + M_PI);
    float refAngle4 = mapAngle(refAngle1 + (3 * M_PI) / 2);
    float refAngles[4] = { refAngle1, refAngle2, refAngle3, refAngle4 };

    // Find furthest point from the centroid for each of the remaining 3 quadrants
    for (int i = 0; i < contour.size(); i++)
    {
        // Compute angle between x axis and line from centroid to point
        float angle = atan2(contour[i].y - centroid.y, contour[i].x - centroid.x);

        // Check if further than furthest point found so far for the given quadrant
        for (int j = 1; j < 4; ++j)
        {
            // Check if in quadrant and if furthest point from centroid for that quadrant
            if (abs(mapAngle(angle - refAngles[j])) <= (M_PI / 4) && distances[i] > maxDist[j])
            {
                maxDist[j] = distances[i];
                corners[j] = contour[i];
                break;
            }
        }
    }

    return corners;
}

void Vision::isolateRectangle(const std::vector<cv::Point>& corners, const cv::Mat& sourceImage, cv::Mat& isolatedImage, cv::Mat& homographyMatrix) const
{
    // Define destination points in isolation image
    std::vector<cv::Point> isolatedImagePoints;
    isolatedImagePoints.push_back(cv::Point(0, 0));
    isolatedImagePoints.push_back(cv::Point(isolatedImage.cols - 1, 0));
    isolatedImagePoints.push_back(cv::Point(isolatedImage.cols - 1, isolatedImage.rows - 1));
    isolatedImagePoints.push_back(cv::Point(0, isolatedImage.rows - 1));

    // Compute homography matrix and isolate and warp rectangle to isolation image
    homographyMatrix = cv::findHomography(corners, isolatedImagePoints);
    warpPerspective(sourceImage, isolatedImage, homographyMatrix, isolatedImage.size());
}

int Vision::processFiducial(const cv::Mat& inputImage, cv::Mat& outputImage) const
{
    // Check if image conforms to isolated fiducial specification
    if (inputImage.rows != fiducialWidth || inputImage.cols != fiducialHeight)
    {
        emit log(Message(MessageType::ERROR_LOG, "Vision System", "Image does not conform to isolated fiducial specification"));
        return -1;
    }

    // Check if image A is a binary image
    if (inputImage.type() != CV_8UC1)
    {
        emit log(Message(MessageType::ERROR_LOG, "Vision System", "Input image is not a binary image"));
        return -1;
    }

    // Initialize output image
    inputImage.copyTo(outputImage);

    // Rotate image until correct orientation is found by alignment with 3 orientation reference blocks
    int rotationCount = 0;
    int refBlock1 = outputImage.at<uchar>(fiducialSamplePoints[0]);
    int refBlock2 = outputImage.at<uchar>(fiducialSamplePoints[6]);
    int refBlock3 = outputImage.at<uchar>(fiducialSamplePoints[8]);
    while (refBlock1 != 0 || refBlock2 != 0 || refBlock3 != 255)
    {
        // Check if all orientations have been tested
        rotationCount++;
        if (rotationCount == 4)
            return -1;

        // Rotate to new orientation
        cv::rotate(outputImage, outputImage, cv::ROTATE_90_CLOCKWISE);

        // Sample orientation reference blocks
        refBlock1 = outputImage.at<uchar>(fiducialSamplePoints[0]);
        refBlock2 = outputImage.at<uchar>(fiducialSamplePoints[6]);
        refBlock3 = outputImage.at<uchar>(fiducialSamplePoints[8]);
    }

    // Get fiducial identifier by mapping fiducial blocks to binary bits
    int bits[6];
    bits[0] = outputImage.at<uchar>(fiducialSamplePoints[1]) ? 1 : 0;
    bits[1] = outputImage.at<uchar>(fiducialSamplePoints[2]) ? 1 : 0;
    bits[2] = outputImage.at<uchar>(fiducialSamplePoints[3]) ? 1 : 0;
    bits[3] = outputImage.at<uchar>(fiducialSamplePoints[4]) ? 1 : 0;
    bits[4] = outputImage.at<uchar>(fiducialSamplePoints[5]) ? 1 : 0;
    bits[5] = outputImage.at<uchar>(fiducialSamplePoints[7]) ? 1 : 0;

    int identifier = 0;
    for (int i = 0; i < 6; ++i)
        identifier |= bits[i] << i;


    //cv::imshow("Display Name", outputImage);
    //cv::waitKey(0);

    return identifier;
}

float Vision::mapAngle(float angle) const
{
    if (angle > M_PI)
        angle -= 2 * M_PI;
    else if (angle <= -M_PI)
        angle += 2 * M_PI;

    return angle;
}

cv::Point3i Vision::projectImagePoint(const cv::Point2d& imagePoint, double z) const
{
    // Check that vision system is calibrated
    if (!calibrated)
        return cv::Point3i(0, 0, 0);

	// Form homogenous image point
	cv::Point3d imagePointH(imagePoint.x, imagePoint.y, 1);

	// Compute left and right components of equation
	cv::Mat leftMatrix = rotationMatrix.inv() * cameraMatrix.inv() * cv::Mat(imagePointH, false);
	cv::Mat rightMatrix = rotationMatrix.inv() * translationVector;

	// Compute world point
	double s = (z + rightMatrix.at<double>(2)) / leftMatrix.at<double>(2);
	cv::Mat worldPointMat = leftMatrix * s - rightMatrix;
	cv::Point3d worldPoint(worldPointMat.at<double>(0), worldPointMat.at<double>(1), worldPointMat.at<double>(2));

	return worldPoint;
}

cv::Point Vision::projectWorldPoint(const cv::Point3d& worldPoint) const
{
    // Check that vision system is calibrated
    if (!calibrated)
        return cv::Point(0, 0);

	// Compute homogenous image point
	cv::Mat imagePointMat = cameraMatrix * (rotationMatrix * cv::Mat(worldPoint, false) + translationVector);

	// Normalize image point
	double s = imagePointMat.at<double>(2);
	cv::Point2d imagePoint = cv::Point2d(imagePointMat.at<double>(0) / s, imagePointMat.at<double>(1) / s);

	return imagePoint;
}
