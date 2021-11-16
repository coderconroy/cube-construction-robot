#include "Vision.h"
#include <iostream>
#include <string>

// Threshold parameters
int thresh = 80;
int maxThresh = 255;
int blurSize = 1;
int maxBlurSize = 20;

// Fiducials
float fiducialThresh = 0.7f;
int fiducialWidth = 128;
int fiducialHeight = 128;

// Bounding box parameters
bool showCoords = true;
double areaThreshold = 1300;

Vision::Vision(QObject* parent) : QObject(parent)
{
    //fiducialWorldPoints.insert(10, cv::Point3i(-32, -397, 0));
    //fiducialWorldPoints.insert(0, cv::Point3i(944, -383, 0));
    //fiducialWorldPoints.insert(50, cv::Point3i(-277, -63, 0));
    //fiducialWorldPoints.insert(11, cv::Point3i(1188, -58, 0));
    //fiducialWorldPoints.insert(3, cv::Point3i(-278, 984, 0));
    //fiducialWorldPoints.insert(41, cv::Point3i(1186, 979, 0));
    //fiducialWorldPoints.insert(6, cv::Point3i(-36, 1316, 0));
    //fiducialWorldPoints.insert(37, cv::Point3i(951, 1320, 0));

    // Initialize world point of each fiducial
    //fiducialWorldPoints.insert(10, cv::Point3i(-32, -387, 0));
    //fiducialWorldPoints.insert(0, cv::Point3i(944, -377, 0));
    //fiducialWorldPoints.insert(50, cv::Point3i(-277, -53, 0));
    //fiducialWorldPoints.insert(11, cv::Point3i(1188, -52, 0));
    //fiducialWorldPoints.insert(3, cv::Point3i(-278, 986, 0));
    //fiducialWorldPoints.insert(41, cv::Point3i(1188, 981, 0));
    //fiducialWorldPoints.insert(6, cv::Point3i(-36, 1318, 0));
    //fiducialWorldPoints.insert(37, cv::Point3i(953, 1322, 0));

    fiducialWorldPoints.insert(10, cv::Point3i(-27, -370, 0));
    fiducialWorldPoints.insert(0, cv::Point3i(946, -379, 0));
    fiducialWorldPoints.insert(50, cv::Point3i(-268, -57, 0));
    fiducialWorldPoints.insert(11, cv::Point3i(1188, -54, 0));
    fiducialWorldPoints.insert(3, cv::Point3i(-278, 986, 0));
    fiducialWorldPoints.insert(41, cv::Point3i(1188, 981, 0));
    fiducialWorldPoints.insert(6, cv::Point3i(-36, 1318, 0));
    fiducialWorldPoints.insert(37, cv::Point3i(953, 1322, 0));

    // Initialize camera matrix and distortion coefficients
    cameraMatrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    distCoeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type);

    // Initialize coordinates of bounding box for computer vision region of interest in world coordinates
    visionBoundBox[0] = ROBOT_X_MIN - 340;
    visionBoundBox[1] = ROBOT_X_MAX + 240;
    visionBoundBox[2] = ROBOT_Y_MIN - 100;
    visionBoundBox[3] = ROBOT_Y_MAX + 260;
}

void Vision::processScene(const cv::Mat& image, bool calibrate, std::vector<cv::Point3i>* sourceCentroids, 
    std::vector<cv::Point3i>* structCentroids)
{
    // Reset vision system to uncalibrated state
    if (calibrate)
        calibrated = false;

    // Reset image contour containers
    fiducialContours.clear();
    cubeContours.clear();
    sourceCubeContours.clear();
    structCubeContours.clear();

    // Reset fiducial stage images
    fiducialImages.clear();
    annotatedFiducialImages.clear();

    // Process image
    cv::Mat processImage;

    // Convert to grayscale and blur
    cv::cvtColor(image, processImage, cv::COLOR_BGR2GRAY);
    cv::blur(processImage, processImage, cv::Size(blurSize + 1, blurSize + 1));
    processImage.copyTo(blurredImage);

    // Apply binary threshold to image
    cv::threshold(processImage, processImage, thresh, maxThresh, cv::THRESH_BINARY);
    processImage.copyTo(thresholdImage);

    // Apply contour detection
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(processImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Plot contours for contour image
    cv::cvtColor(processImage, contourImage, cv::COLOR_GRAY2BGR);
    cv::drawContours(contourImage, contours, -1, cv::Scalar(0, 255, 0), 4);

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
            cv::Mat annotatedFiducialImage(fiducialWidth, fiducialHeight, CV_8UC1);
            cv::Mat homographyMatrix;
            isolateRectangle(corners, processImage, isolatedImage, homographyMatrix);
            cv::threshold(isolatedImage, isolatedImage, thresh, maxThresh, cv::THRESH_BINARY);

            // Process fiducial
            int fiducialId = identifyFiducial(isolatedImage, isolatedImage, annotatedFiducialImage);

            // Add to fiducial contour list if fiducial
            // TODO: Add further checks to confirm fiducial nature
            if (fiducialId >= 0)
            {
                FiducialContour fiducial;
                fiducial.id = fiducialId;
                fiducial.centroid = centroid;
                fiducial.contour = contour;
                fiducial.corners = corners;
                fiducial.homographyMatrix = homographyMatrix;
                fiducialContours.push_back(fiducial);

                fiducialImages.push_back(isolatedImage);
                annotatedFiducialImages.push_back(annotatedFiducialImage);
            }
            // Assume contour is cube and add to cube contour list
            // TODO: Add further checks to confirm cube nature
            else
            {
                CubeContour cube;
                cube.centroid = centroid;
                cube.contour = contour;
                cube.corners = corners;
                cubeContours.push_back(cube);
            }
        }
    }

    // Use fiducials to calibrate for rotation and translation matrices
    if (calibrate)
    {
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
        // At least four point correspondences are required to solve for the extrinsic parameters
        if (worldPoints.size() >= 4)
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

    // The following image processing requires a calibrated system
    if (!calibrated)
        return;

    // Remove centroids that do not fall within the computer vision region of interest
    // The computer vision region of interest bounding box is defined on the base plane so all centroids are projected to
    // and evaluated on this plane
    for (std::vector<CubeContour>::iterator iter = cubeContours.begin(); iter != cubeContours.end();)
    {
        // Project centroid to the base plane
        cv::Point3i worldCentroid = projectImagePoint(iter->centroid, 0);

        // Identify if the cube is located within the compute vision region of interest bounding box
        if (worldCentroid.x >= visionBoundBox[0] && worldCentroid.x <= visionBoundBox[1]
            && worldCentroid.y >= visionBoundBox[2] && worldCentroid.y <= visionBoundBox[3])
        {
            // Increment iterator since no contour is removed from the cube contour list
            ++iter;
        }
        else
        {
            // Remove contour as it does not fall within the compute vision bounding region of interest
            iter = cubeContours.erase(iter);
        }
    }

    // Determine if any of the non-fiducial contours are artifacts originating from source cubes
    // The contour is considered a source cube artifact if its centroid is sufficiently close to a source cube centroid
    if (sourceCentroids != Q_NULLPTR)
    {
        for (std::vector<CubeContour>::iterator iter = cubeContours.begin(); iter != cubeContours.end();)
        {
            bool sourceContour = false;
            for (int j = 0; j < sourceCentroids->size(); ++j)
            {
                // Project contour centroid to the world frame in the source cube plane
                cv::Point3i worldCentroid = projectImagePoint(iter->centroid, -sourceCentroids->at(j).z);
                cv::Point3i sourceCentroid = cv::Point3i(sourceCentroids->at(j).x, sourceCentroids->at(j).y, -sourceCentroids->at(j).z);

                // Compute Euclidean distance between points in the world space
                float dist = computeEuclidDist(worldCentroid, sourceCentroid);

                // Check if centroid is sufficiently close to source cube centroid
                if (dist <= 64)
                {
                    // Classify contour as source cube contour
                    sourceContour = true;
                    sourceCubeContours.push_back(*iter);
                    iter = cubeContours.erase(iter);
                    break;
                }
            }
            // Increment iterator if no contour was removed from the cube contour list
            if (!sourceContour)
                ++iter;
        }
    }

    // Determine if any of the non-fiducial contours are artifacts originating from structure cubes
    // The contour is considered a source cube artifact if its centroid is sufficiently close to a structure cube centroid
    if (structCentroids != Q_NULLPTR)
    {
        for (std::vector<CubeContour>::iterator iter = cubeContours.begin(); iter != cubeContours.end();)
        {
            bool structContour = false;
            for (int j = 0; j < structCentroids->size(); ++j)
            {
                // Project contour centroid to the world frame in the structure cube plane
                cv::Point3i worldCentroid = projectImagePoint(iter->centroid, -structCentroids->at(j).z);
                cv::Point3i structCentroid = cv::Point3i(structCentroids->at(j).x, structCentroids->at(j).y, -structCentroids->at(j).z);

                // Compute Euclidean distance between points in the world space
                float dist = computeEuclidDist(worldCentroid, structCentroid);

                // Check if centroid is sufficiently close to source cube centroid
                if (dist <= 64)
                {
                    // Classify contour as source cube contour
                    structContour = true;
                    structCubeContours.push_back(*iter);
                    iter = cubeContours.erase(iter);
                    break;
                }
            }
            // Increment iterator if no contour was removed from the cube contour list
            if (!structContour)
                ++iter;
        }
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

        //cv::Point3i worldPoint = projectImagePoint(f.centroid, 0); // Project image point to world 
        //QString coordinateText = "(" + QString::number(worldPoint.x) + ", " + QString::number(worldPoint.y) + ")";
        //cv::Point coordinateTextPoint(f.centroid.x - 70, f.centroid.y + 50);
        //cv::putText(image, coordinateText.toStdString(), coordinateTextPoint, cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

        // Plot identifier
        cv::Point textPoint(f.centroid.x - 20, f.centroid.y + 10);
        cv::putText(image, std::to_string(f.id), textPoint, cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(0, 0, 255), 2);

        //// Plot centroid
        //circle(image, f.centroid, 4, cv::Scalar(255, 0, 128), -1, cv::LINE_AA);

        // Plot corners
        for (int j = 0; j < f.corners.size(); ++j)
            circle(image, f.corners[j], 4, cv::Scalar(0, 255, 255), -1, cv::LINE_AA);
    }
}

void Vision::plotCubeInfo(cv::Mat& image)
{
    // Plot independent cube information
    for (int i = 0; i < cubeContours.size(); ++i)
    {
        // Get contour
        CubeContour c = cubeContours[i];

        // Plot contour
        std::vector<std::vector<cv::Point>> contours = { c.contour };
        //cv::drawContours(image, contours, 0, cv::Scalar(0, 255, 0), 4);

        // Plot world coordinate text
        cv::Point3i worldPoint = projectImagePoint(c.centroid, -64); // Project image point to world 
        QString coordinateText = "(" + QString::number(worldPoint.x) + ", " + QString::number(worldPoint.y) + ")";
        cv::Point coordinateTextPoint(c.centroid.x - 60, c.centroid.y + 40);
        cv::putText(image, coordinateText.toStdString(), coordinateTextPoint, cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

        // Plot orientation text
        float angle = computeCubeZRotation(c.corners, c.centroid, -64);
        QString angleText = QString::number(round(angle / M_PI * 180)) + " deg";
        cv::Point angleTextPoint(c.centroid.x - 50, c.centroid.y + 70);
        cv::putText(image, angleText.toStdString(), angleTextPoint, cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

        // Plot orientation reference line
        int lineLength = 64;
        cv::Point3i worldCentroid = projectImagePoint(c.centroid, -64);
        cv::Point3i xRefPoint = worldCentroid + cv::Point3i(lineLength, 0, 0);
        cv::Point3i angleRefPoint = worldCentroid + cv::Point3i(lineLength * cos(angle), lineLength * sin(angle), 0);
        cv::line(image, projectWorldPoint(worldCentroid), projectWorldPoint(xRefPoint), cv::Scalar(0, 255, 0), 3, cv::LINE_8);
        cv::line(image, projectWorldPoint(worldCentroid), projectWorldPoint(angleRefPoint), cv::Scalar(0, 255, 0), 3, cv::LINE_8);

        // Plot centroid
        circle(image, c.centroid, 4, cv::Scalar(255, 0, 128), -1, cv::LINE_AA);

        // Plot corners
        for (int j = 0; j < c.corners.size(); ++j)
            circle(image, c.corners[j], 4, cv::Scalar(255, 255, 0), -1, cv::LINE_AA);
    }
}

void Vision::plotSourceCubeInfo(cv::Mat& image)
{
    // Plot source cube information
    for (int i = 0; i < sourceCubeContours.size(); ++i)
    {
        // Get contour
        CubeContour c = sourceCubeContours[i];

        // Plot contour
        std::vector<std::vector<cv::Point>> contours = { c.contour };
        cv::drawContours(image, contours, 0, cv::Scalar(255, 0, 0), 4);

        //// Plot centroid
        //circle(image, c.centroid, 4, cv::Scalar(255, 0, 128), -1, cv::LINE_AA);
    }
}

void Vision::plotStructCubeInfo(cv::Mat& image)
{
    // Plot source cube information
    for (int i = 0; i < structCubeContours.size(); ++i)
    {
        // Get contour
        CubeContour c = structCubeContours[i];

        // Plot contour
        std::vector<std::vector<cv::Point>> contours = { c.contour };
        cv::drawContours(image, contours, 0, cv::Scalar(0, 255, 0), 4);

        //// Plot centroid
        //circle(image, c.centroid, 4, cv::Scalar(255, 0, 128), -1, cv::LINE_AA);
    }
}

void Vision::plotWorkspaceBoundBox(cv::Mat& image)
{
    // Project bounding box world coordinates to image coordinates
    cv::Point imageCoordinatesL[4]; // Lower bounding box
    cv::Point imageCoordinatesH[4]; // Upper bounding box

    imageCoordinatesL[0] = projectWorldPoint(cv::Point3i(ROBOT_X_MIN, ROBOT_Y_MIN, 0));
    imageCoordinatesL[1] = projectWorldPoint(cv::Point3i(ROBOT_X_MIN, ROBOT_Y_MAX, 0));
    imageCoordinatesL[2] = projectWorldPoint(cv::Point3i(ROBOT_X_MAX, ROBOT_Y_MAX, 0));
    imageCoordinatesL[3] = projectWorldPoint(cv::Point3i(ROBOT_X_MAX, ROBOT_Y_MIN, 0));

    imageCoordinatesH[0] = projectWorldPoint(cv::Point3i(ROBOT_X_MIN, ROBOT_Y_MIN, 6 * -64));
    imageCoordinatesH[1] = projectWorldPoint(cv::Point3i(ROBOT_X_MIN, ROBOT_Y_MAX, 6 * -64));
    imageCoordinatesH[2] = projectWorldPoint(cv::Point3i(ROBOT_X_MAX, ROBOT_Y_MAX, 6 * -64));
    imageCoordinatesH[3] = projectWorldPoint(cv::Point3i(ROBOT_X_MAX, ROBOT_Y_MIN, 6 * -64));

    // Plot bounding box
    for (int i = 0; i < 4; ++i)
    {
        cv::line(image, imageCoordinatesL[i], imageCoordinatesL[(i + 1) % 4], cv::Scalar(255, 255, 0), 3, cv::LINE_8);
        cv::line(image, imageCoordinatesH[i], imageCoordinatesH[(i + 1) % 4], cv::Scalar(255, 255, 0), 3, cv::LINE_8);
        cv::line(image, imageCoordinatesL[i], imageCoordinatesH[i], cv::Scalar(255, 255, 0), 3, cv::LINE_8);
    }
}

void Vision::plotVisionBoundBox(cv::Mat& image)
{
    // Project bounding box world coordinates to image coordinates
    cv::Point imageCoordinatesL[4]; // Lower bounding box

    imageCoordinatesL[0] = projectWorldPoint(cv::Point3i(visionBoundBox[0], visionBoundBox[2], 0));
    imageCoordinatesL[1] = projectWorldPoint(cv::Point3i(visionBoundBox[0], visionBoundBox[3], 0));
    imageCoordinatesL[2] = projectWorldPoint(cv::Point3i(visionBoundBox[1], visionBoundBox[3], 0));
    imageCoordinatesL[3] = projectWorldPoint(cv::Point3i(visionBoundBox[1], visionBoundBox[2], 0));

    // Plot bounding box
    for (int i = 0; i < 4; ++i) 
        cv::line(image, imageCoordinatesL[i], imageCoordinatesL[(i + 1) % 4], cv::Scalar(255, 0, 128), 3, cv::LINE_8);
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
    // The reference angles are defined in an anti-clockwise direction (i.e. increasing angle)
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

int Vision::identifyFiducial(const cv::Mat& inputImage, cv::Mat& outputImage, cv::Mat& annotatedFiducial) const
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

    //cv::imshow("Fiducial", outputImage);
    //cv::waitKey();

    // Rotate image until correct orientation is found by alignment with 3 orientation reference blocks
    int rotationCount = 0;
    int refBlock1 = classifyFiducialSquare(outputImage, 0, 0);
    int refBlock2 = classifyFiducialSquare(outputImage, 2, 0);
    int refBlock3 = classifyFiducialSquare(outputImage, 2, 2);
    while (refBlock1 != 0 || refBlock2 != 0 || refBlock3 != 1)
    {
        // Check if all orientations have been tested
        rotationCount++;
        if (rotationCount == 4)
            return -1;

        // Rotate to new orientation
        cv::rotate(outputImage, outputImage, cv::ROTATE_90_CLOCKWISE);

        // Sample orientation reference blocks
        refBlock1 = classifyFiducialSquare(outputImage, 0, 0);
        refBlock2 = classifyFiducialSquare(outputImage, 2, 0);
        refBlock3 = classifyFiducialSquare(outputImage, 2, 2);
    }

    // Get fiducial identifier by mapping fiducial blocks to binary bits
    int bits[6];
    bits[0] = classifyFiducialSquare(outputImage, 0, 1);
    bits[1] = classifyFiducialSquare(outputImage, 0, 2);
    bits[2] = classifyFiducialSquare(outputImage, 1, 0);
    bits[3] = classifyFiducialSquare(outputImage, 1, 1);
    bits[4] = classifyFiducialSquare(outputImage, 1, 2);
    bits[5] = classifyFiducialSquare(outputImage, 2, 1);

    int identifier = 0;
    for (int i = 0; i < 6; ++i)
    {
        // Check if bit was successfully classified
        if (bits[i] == -1)
            return -1;

        // Add bit to identifier
        identifier |= bits[i] << i;
    }

    // Annotate fiducial image with grid and binary values
    outputImage.copyTo(annotatedFiducial);
    cv::cvtColor(annotatedFiducial, annotatedFiducial, cv::COLOR_GRAY2RGB);
    int gridLinePos[4] = { 16, 48, 79, 111 };
    for (int i = 0; i < 4; i++)
    {
        cv::line(annotatedFiducial, cv::Point(gridLinePos[i], gridLinePos[0]), cv::Point(gridLinePos[i], gridLinePos[3]), cv::Scalar(255, 0, 0), 2, cv::LINE_8);
        cv::line(annotatedFiducial, cv::Point(gridLinePos[0], gridLinePos[i]), cv::Point(gridLinePos[3], gridLinePos[i]), cv::Scalar(255, 0, 0), 2, cv::LINE_8);
    }

    int xOffset = 10;
    int yOffset = 25;
    cv::putText(annotatedFiducial, "X", cv::Point(gridLinePos[0] + xOffset, gridLinePos[0] + yOffset), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    cv::putText(annotatedFiducial, std::to_string(bits[2]), cv::Point(gridLinePos[0] + xOffset, gridLinePos[1] + yOffset), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
    cv::putText(annotatedFiducial, "X", cv::Point(gridLinePos[0] + xOffset, gridLinePos[2] + yOffset), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    cv::putText(annotatedFiducial, std::to_string(bits[0]), cv::Point(gridLinePos[1] + xOffset, gridLinePos[0] + yOffset), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
    cv::putText(annotatedFiducial, std::to_string(bits[3]), cv::Point(gridLinePos[1] + xOffset, gridLinePos[1] + yOffset), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
    cv::putText(annotatedFiducial, std::to_string(bits[5]), cv::Point(gridLinePos[1] + xOffset, gridLinePos[2] + yOffset), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
    cv::putText(annotatedFiducial, std::to_string(bits[1]), cv::Point(gridLinePos[2] + xOffset, gridLinePos[0] + yOffset), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
    cv::putText(annotatedFiducial, std::to_string(bits[4]), cv::Point(gridLinePos[2] + xOffset, gridLinePos[1] + yOffset), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
    cv::putText(annotatedFiducial, "X", cv::Point(gridLinePos[2] + xOffset, gridLinePos[2] + yOffset), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(0, 255, 0), 2);

    return identifier;
}

int Vision::classifyFiducialSquare(const cv::Mat& fiducialImage, int row, int col) const
{
    int squareLength = 32; // Side length of gird square in pixels
    int startPixel = 16; // Location of first pixel in first square
    float threshold = 0.7; // Minimum proportion of square of one type required for classification (0.5, 1]
    int padding = 3; // Number of pixels of padding that are not included in square sum


    // Count number of zero and one pixels in fiducial square
    int zeroSum = 0;
    int oneSum = 0;
    for (int i = startPixel + row * squareLength + padding; i < startPixel + (row + 1) * squareLength - padding; i++)
    {
        for (int j = startPixel + col * squareLength + padding; j < startPixel + (col + 1) * squareLength - padding; j++)
        {
            if (fiducialImage.at<uchar>(cv::Point(j, i)) == 0)
                zeroSum++;
            else if (fiducialImage.at<uchar>(cv::Point(j, i)) == 255)
                oneSum++;
        }
    }

    // Classify pixel
    int totalPixels = pow(squareLength - 2 * padding, 2);
    if ((float)zeroSum / totalPixels > threshold)
        return 0;
    else if ((float)oneSum / totalPixels > threshold)
        return 1;
    else
        return -1;
}

float Vision::mapAngle(float angle) const
{
    if (angle > M_PI)
        angle -= 2 * M_PI;
    else if (angle <= -M_PI)
        angle += 2 * M_PI;

    return angle;
}

float Vision::mapToCubeAngle(float angle) const
{
    while (angle > M_PI / 4)
        angle -= M_PI / 2;

    while (angle <= - M_PI / 4)
        angle += M_PI / 2;

    return angle;
}

float Vision::computeCubeZRotation(const std::vector<cv::Point>& corners, const cv::Point centroid, const int z) const
{
    // Project centroid point to the world frame
    cv::Point3i worldCentroid = projectImagePoint(centroid, z);

    // Compute cube z rotation estimate using each corner point in the world frame
    cv::Point3i worldCorners[4];
    float angles[4];
    for (int i = 0; i < 4; ++i)
    {
        // Project the corners from the image frame to the given xy plane in the world frame
        worldCorners[i] = projectImagePoint(corners[i], z);

        // Compute angle formed by the line from the top cube face centroid to each corner with the x-axis in radians
        // Rotate 90 degrees clockwise to align with the line formed by the first corner and centroid and map to range(- PI, PI]
        // This assumes the corners are specified in an anti-clockwise direction
        angles[i] = atan2(worldCorners[i].y - worldCentroid.y, worldCorners[i].x - worldCentroid.x) - (M_PI * i / 2);

        // Covert angle to that between the x-axis and the line from the centroid perpendicular to the side of the cube and map to (- PI, PI]
        angles[i] = mapAngle(angles[i] + M_PI / 4);
    }

    // Compute average of angle estimates using the individual corners, map to (- PI / 4, PI / 4] and return single angle estimate
    float ySum = 0;
    float xSum = 0;
    for (int i = 0; i < 4; ++i)
    {
        xSum += cos(angles[i]);
        ySum += sin(angles[i]);
    }

    float angle = mapToCubeAngle(atan2(ySum / 4, xSum / 4));

    return -angle;
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

double Vision::computeEuclidDist(const cv::Point3i& pointA, const cv::Point3i& pointB) const
{
    return sqrt(pow(pointA.x - pointB.x, 2) + pow(pointA.y - pointB.y, 2) + pow(pointA.z - pointB.z, 2));
}

cv::Mat Vision::getBlurredImage() const
{
    return blurredImage;
}

cv::Mat Vision::getThresholdedImage() const
{
    return thresholdImage;
}

cv::Mat Vision::getContourImage() const
{
    return contourImage;
}

std::vector<cv::Mat> Vision::getFiducialImages() const
{
    return fiducialImages;
}

std::vector<cv::Mat> Vision::getAnnotatedFiducialImages() const
{
    return annotatedFiducialImages;
}

std::vector<cv::Point3i> Vision::getCubeCentroids(const int z) const
{
    // Compile list of cube centroids from the cube contour list for a given plane in the world frame
    std::vector<cv::Point3i> centroids;
    for (int i = 0; i < cubeContours.size(); ++i)
    {
        // Project centroid to the world frame and add to centroid list
        cv::Point3i worldCentroid = projectImagePoint(cubeContours[i].centroid, -z);
        worldCentroid.z = -worldCentroid.z;
        centroids.push_back(worldCentroid);
    }

    return centroids;
}

std::vector<float> Vision::getCubeRotations(const int z) const
{
    // Compile list of cube centroids from the cube contour list for a given plane in the world frame
    std::vector<float> rotations;
    for (int i = 0; i < cubeContours.size(); ++i)
    {
        // Get contour
        CubeContour c = cubeContours[i];

        // Compute cube rotation and add to the roatations list
        float angle = computeCubeZRotation(c.corners, c.centroid, -z);
        rotations.push_back(angle);
    }

    return rotations;
}