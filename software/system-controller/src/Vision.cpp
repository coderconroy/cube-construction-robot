#include "Vision.h"
#include <iostream>

// Threshold parameters
int thresh = 80;
int maxThresh = 255;
int blurSize = 1;
int maxBlurSize = 20;

// Bounding box parameters
int left = 480;
int right = 1360;
int top = 150;
int bottom = 950;
bool showCoords = true;
double areaThreshold = 1300;

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

void Vision::calibrate(cv::Mat& input)
{
    // Initialize intermediate image matrices
    cv::Mat processImage;
    cv::Mat output;
    input.copyTo(output);

    // Process image
    cv::cvtColor(input, processImage, cv::COLOR_BGR2GRAY);
    cv::blur(processImage, processImage, cv::Size(blurSize + 1, blurSize + 1));
    cv::threshold(processImage, processImage, thresh, maxThresh, cv::THRESH_BINARY);

    // Apply contour detection
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(processImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Initialize cube and fiducial lists
    std::vector<std::vector<cv::Point>> cubes;
    std::vector<std::vector<cv::Point>> fiducials;

    // Process contours for cubes and fiducials
    for (int i = 0; i < contours.size(); i++)
    {
        // Get contour
        std::vector<cv::Point> contour = contours[i];

        // Get contour centroid and area
        cv::Point2d centroid = getCentroid(contour);
        double area = cv::contourArea(contours[i]);

        // Process contours of significant size in bounding box 
        if (centroid.x > left && centroid.x < right && centroid.y > top && centroid.y < bottom && area > areaThreshold)
        {
            // Find corners
            std::vector<cv::Point> corners = findSquareCorners(contour);


            // Plot corners
            circle(output, corners[0], 4, cv::Scalar(0, 255, 255), -1, cv::LINE_AA);
            circle(output, corners[1], 4, cv::Scalar(0, 255, 255), -1, cv::LINE_AA);
            circle(output, corners[2], 4, cv::Scalar(0, 255, 255), -1, cv::LINE_AA);
            circle(output, corners[3], 4, cv::Scalar(0, 255, 255), -1, cv::LINE_AA);
            circle(output, centroid, 4, cv::Scalar(255, 0, 0), -1, cv::LINE_AA);

            //if (showCoords)
            //{
            //    cv::putText(input,
            //        "(" + std::to_string((int)momentCentroids[i].x) + ", " + std::to_string((int)momentCentroids[i].y) + ")",
            //        cv::Point2f(momentCentroids[i].x - 90, momentCentroids[i].y + 50), // Coordinates
            //        cv::FONT_HERSHEY_PLAIN, // Font
            //        2, // Scale. 2.0 = 2x bigger
            //        cv::Scalar(255, 255, 255), // BGR Color
            //        2, // Line Thickness (Optional)
            //        cv::LINE_AA); // Anti-alias (Optional, see version note)


            //}
        }
    }

    // Display result
    cv::resize(output, output, cv::Size(), 0.75, 0.75);
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);// Create a window for display.
    cv::imshow("Display window", output);
    cv::waitKey(0);
}

cv::Point Vision::getCentroid(std::vector<cv::Point>& contour) const
{
    cv::Moments contourMoments = moments(contour, false);
    cv::Point centroid = cv::Point2f(contourMoments.m10 / contourMoments.m00, contourMoments.m01 / contourMoments.m00);
    return centroid;
}

std::vector<cv::Point> Vision::findSquareCorners(std::vector<cv::Point>& contour) const
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

float Vision::mapAngle(float angle) const
{
    if (angle > M_PI)
        angle -= 2 * M_PI;
    else if (angle <= -M_PI)
        angle += 2 * M_PI;

    return angle;
}

cv::Point3d Vision::projectImagePoint(const cv::Point2d& imagePoint, double z) const
{
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

cv::Point2d Vision::projectWorldPoint(cv::Point3d& worldPoint) const
{
	// Compute homogenous image point
	cv::Mat imagePointMat = cameraMatrix * (rotationMatrix * cv::Mat(worldPoint, false) + translationVector);

	// Normalize image point
	double s = imagePointMat.at<double>(2);
	cv::Point2d imagePoint = cv::Point2d(imagePointMat.at<double>(0) / s, imagePointMat.at<double>(1) / s);

	return imagePoint;
}

void Vision::temp()
{
    cv::Mat input;
    cv::Mat frame;
    input.copyTo(frame);

    // Process image
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    cv::blur(frame, frame, cv::Size(blurSize + 1, blurSize + 1));
    cv::threshold(frame, frame, thresh, maxThresh, cv::THRESH_BINARY);

    // Apply contour detection
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(frame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Get the moment of each contour
    std::vector<cv::Moments> contourMoments(contours.size());
    for (int i = 0; i < contours.size(); i++)
        contourMoments[i] = moments(contours[i], false);

    // Get the centroid of each contour moment
    std::vector<cv::Point2f> momentCentroids(contours.size());
    for (int i = 0; i < contours.size(); i++)
        momentCentroids[i] = cv::Point2f(contourMoments[i].m10 / contourMoments[i].m00, contourMoments[i].m01 / contourMoments[i].m00);

    // Draw and label each moment centroid
    for (int i = 0; i < contours.size(); i++)
    {
        cv::Point2d centroid = momentCentroids[i];
        double area = cv::contourArea(contours[i]);
        if (centroid.x > left && centroid.x < right && centroid.y > top && centroid.y < bottom && area > areaThreshold)
        {
            circle(input, momentCentroids[i], 4, cv::Scalar(255, 0, 0), -1, cv::LINE_AA);
            std::vector<std::vector<cv::Point>> contour;
            contour.push_back(contours[i]);

            cv::drawContours(input, contour, -1, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);

            double cx = momentCentroids[i].x;
            double cy = momentCentroids[i].y;
            double maxDist = 0;
            double maxIndex = 0;
            for (int j = 0; j < contours[i].size(); j++)
            {
                double px = contours[i][j].x;
                double py = contours[i][j].y;
                double dist = sqrt(pow(px - cx, 2) + pow(py - cy, 2));

                if (dist > maxDist)
                {
                    maxDist = dist;
                    maxIndex = j;
                }
            }

            circle(input, contours[i][maxIndex], 4, cv::Scalar(0, 255, 255), -1, cv::LINE_AA);

            if (showCoords)
            {
                cv::putText(input,
                    "(" + std::to_string((int)momentCentroids[i].x) + ", " + std::to_string((int)momentCentroids[i].y) + ")",
                    cv::Point2f(momentCentroids[i].x - 90, momentCentroids[i].y + 50), // Coordinates
                    cv::FONT_HERSHEY_PLAIN, // Font
                    2, // Scale. 2.0 = 2x bigger
                    cv::Scalar(255, 255, 255), // BGR Color
                    2, // Line Thickness (Optional)
                    cv::LINE_AA); // Anti-alias (Optional, see version note)


            }
        }
    }

    // Show principle point
    circle(input, cv::Point2d(cx, cy), 4, cv::Scalar(255, 0, 255), -1, cv::LINE_AA);

    // Draw image box
    cv::Scalar color(255, 255, 0);
    cv::Point tl(left, top), bl(left, bottom), tr(right, top), br(right, bottom);
    int thickness = 2;

    // Line drawn using 8 connected
    // Bresenham algorithm
    line(input, tl, tr, color, thickness, cv::LINE_8);
    line(input, tr, br, color, thickness, cv::LINE_8);
    line(input, br, bl, color, thickness, cv::LINE_8);
    line(input, bl, tl, color, thickness, cv::LINE_8);
}