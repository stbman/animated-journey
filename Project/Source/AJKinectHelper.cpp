//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJKinectHelper.h"
#include "../Source/gfxmanager.h"
#include "../Source/gfxcamera.h"

namespace AJKinectHelper
{
    bool IsValidPoint(cv::Mat depth, XMFLOAT2& center)
    {
        return IsValidPoint(depth, (UINT)center.x, (UINT)center.y);
    }

    bool IsValidPoint(cv::Mat depth, UINT x, UINT y)
    {
        UINT width  = depth.cols;
        UINT height = depth.rows;

        if (   x >= width
            || y >= height)
        {
            return false;
        }

        UINT    offset = (y * width) + x;
        UINT16* data   = (UINT16*)&depth.data[offset * 2];
        UINT16  value  = *data;

        if (   value == 0x0000  // too near
            || value == 0x0FFF  // too far
            || value == 0x1FFF) // unknown
        {
            return false;
        }
        
        return true;
    }

    float GetDepth(cv::Mat depth, UINT x, UINT y)
    {
        if (!IsValidPoint(depth, x, y))
        {
            return -1.0f;
        }

        UINT width = depth.cols;

        UINT    offset = (y * width) + x;
        UINT16* data   = (UINT16*)&depth.data[offset * 2];
        UINT16  value  = *data & 0x0FFF;

        float ratio = AJMathHelper::Cursorize((float)value, 0.0f, (float)(UINT16)(0x0FFF));
        return ratio * 8.0f;
    }

    bool GetCameraPoint(XMFLOAT3& result, cv::Mat cameraPoint, XMFLOAT2& center)
    {
        return GetCameraPoint(result, cameraPoint, (UINT)center.x, (UINT)center.y);
    }

    bool GetCameraPoint(XMFLOAT3& result, cv::Mat cameraPoint, UINT x, UINT y)
    {
        UINT width = cameraPoint.cols;
        UINT height = cameraPoint.rows;

        if (   x >= width
            || y >= height)
        {
            return false;
        }

        UINT              offset = (y * width) + x;
        CameraSpacePoint* data   = (CameraSpacePoint*)cameraPoint.data;
                          data  += offset;
        CameraSpacePoint& value = *data;

        result = XMFLOAT3(value.X, value.Y, value.Z);
        return true;
    }

    bool GetImageColorValue(XMFLOAT3& result, cv::Mat cameraPoint, XMFLOAT2& center)
    {
        return GetImageColorValue(result, cameraPoint, (UINT)center.x, (UINT)center.y);
    }

    bool GetImageColorValue(XMFLOAT3& result, cv::Mat cameraPoint, UINT x, UINT y)
    {
        UINT width = cameraPoint.cols;
        UINT height = cameraPoint.rows;

        if (x >= width
            || y >= height)
        {
            return false;
        }

        UINT              offset = (y * width) + x;
        BYTE* data = (BYTE*)cameraPoint.data;
        data += (offset * 4);
        BYTE& value = *data;

        result = XMFLOAT3((float)value, (float)(value + 1), (float)(value + 2));
        return true;
    }

    bool ProcessImage(cv::Mat& base, cv::Mat& raw, cv::Mat& space, XMFLOAT3& position)
    {
        cv::Mat diff;
        cv::absdiff(raw, base, diff);

        cv::Mat redImg;
        cv::extractChannel(diff, redImg, 2);

        redImg = redImg + cv::Scalar(25, 25, 25);

        int lowValue = 80, highValue = 255;
        cv::Mat threshold;
        cv::threshold(redImg, threshold, lowValue, highValue, CV_THRESH_BINARY);

        int erodelowValue = 5, dilatelowValue = 9;
        cv::Mat erode;
        cv::Mat erodingElement = cv::getStructuringElement(cv::MORPH_ELLIPSE,
            cv::Size(erodelowValue, erodelowValue));
        cv::erode(threshold, erode, erodingElement);

        cv::Mat dilate;
        cv::Mat dilatingElement = getStructuringElement(cv::MORPH_ELLIPSE,
            cv::Size(dilatelowValue, dilatelowValue));
        cv::dilate(erode, dilate, dilatingElement);

        cv::Mat canny;
        cv::Canny(dilate, canny, lowValue, lowValue * 2, 3);

        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;

        cv::findContours(canny, contours, hierarchy, CV_RETR_TREE,
            CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

        XMFLOAT2 centerPoint;
        double largest_area = 0;
        int largest_contour_index = 0;
        cv::Rect largestRect;
        if (contours.size() > 0)
        {
            for (int i = 0; i < contours.size(); i++)
            {

                cv::Rect boundingRectForContour = cv::boundingRect(contours[i]);
            
                // iterate through each contour. 
                double a = boundingRectForContour.area();// contourArea(contours[i], false);  //  Find the area of contour
                if (a > largest_area)
                {
                    largest_area = a;
                    largest_contour_index = i;                //Store the index of largest contour
                    largestRect = cv::boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
                }
            }

            // Calculate center of largest contour using moments 
            cv::Moments moment = cv::moments(contours[largest_contour_index]);
            centerPoint.x = (float)(moment.m10 / moment.m00);
            centerPoint.y = (float)(moment.m01 / moment.m00);
        }

        if (   !AJKinectHelper::GetCameraPoint(position, space, centerPoint)
            || abs(position.x) > 100.0f
            || abs(position.y) > 100.0f
            || abs(position.z) > 100.0f)
        {
            return false;
        }

        return true;
    }

    void PerformCalibrationOnDelta(std::vector<AJPointPairGenerator::PointPair>& first, std::vector<AJPointPairGenerator::PointPair>& second)
    {
        XMFLOAT3 nearCenter, nearLeft, nearRight, nearTop, nearBottom;
        XMFLOAT3 farCenter, farLeft, farRight, farTop, farBottom;

        std::vector<XMFLOAT3> nearAllPoints, nearTopRow, nearBtmRow, nearLeftCol, nearRightCol;
        std::vector<XMFLOAT3> farAllPoints, farTopRow, farBtmRow, farLeftCol, farRightCol;

        GrabPointsFromPPGen(second, nearCenter, nearLeft, nearRight, nearTop, nearBottom,
            nearAllPoints, nearTopRow, nearBtmRow, nearLeftCol, nearRightCol);
        GrabPointsFromPPGen(first, farCenter, farLeft, farRight, farTop, farBottom,
            farAllPoints, farTopRow, farBtmRow, farLeftCol, farRightCol);

        
        AJKinectHelper::ProcessCalibration(nearCenter, nearLeft, nearRight, nearTop, nearBottom,
                                           farCenter, farLeft, farRight, farTop, farBottom,
                                           nearAllPoints, nearTopRow, nearBtmRow, nearLeftCol, nearRightCol,
                                           farAllPoints, farTopRow, farBtmRow, farLeftCol, farRightCol);

    }

    void GrabPointsFromPPGen(std::vector<AJPointPairGenerator::PointPair>& data, XMFLOAT3& center, XMFLOAT3& left, XMFLOAT3& right,
        XMFLOAT3& top, XMFLOAT3& bottom, std::vector<XMFLOAT3> &allPoints, std::vector<XMFLOAT3> &topRow,
        std::vector<XMFLOAT3> &btmRow, std::vector<XMFLOAT3> &leftCol, std::vector<XMFLOAT3> &rightCol)
    {
        UINT dataSize = (UINT)data.size();
        UINT sqrtSize = (UINT)sqrt((double)dataSize);
        UINT sqrtEdge = sqrtSize - 1;
        if ((sqrtSize*sqrtSize) != dataSize)
        {
            return;
        }

        std::vector<XMFLOAT3> avgCenter, avgLeft, avgRight, avgTop, avgBtm;

        for (UINT j = 0; j < 11; ++j)
        for (UINT i = 0; i < 11; ++i)
        {
            UINT value = (j * sqrtSize) + i;
            XMFLOAT3 pos(999.9f, 999.0f, 999.0f);
            if (data[value].m_Found)
            {
                XMStoreFloat3(&pos, data[value].m_Result);

                avgCenter.push_back(pos);

                if (j == 0)
                    avgTop.push_back(pos);
                if (j == sqrtEdge)
                    avgBtm.push_back(pos);
                if (i == 0)
                    avgRight.push_back(pos);
                if (i == sqrtEdge)
                    avgLeft.push_back(pos);
            }
            allPoints.push_back(pos);

            if (j == 0)
                topRow.push_back(pos);
            if (j == sqrtEdge)
                btmRow.push_back(pos);
            if (i == 0)
                rightCol.push_back(pos);
            if (i == sqrtEdge)
                leftCol.push_back(pos);
        }

        center = AJMathHelper::GetAverage(avgCenter);
        left   = AJMathHelper::GetAverage(avgLeft);
        right  = AJMathHelper::GetAverage(avgRight);
        top    = AJMathHelper::GetAverage(avgTop);
        bottom = AJMathHelper::GetAverage(avgBtm);
    }

    void CleanupPairs(std::vector<XMFLOAT3>& nearPos, std::vector<XMFLOAT3>& farPos)
    {
        if (nearPos.size() != farPos.size())
        {
            return;
        }
        std::vector<XMFLOAT3> newNear, newFar;
        for (size_t i = 0; i < nearPos.size(); ++i)
        {
            if (   nearPos[i].x >  100.0f
                || nearPos[i].x < -100.0f
                || nearPos[i].y >  100.0f
                || nearPos[i].y < -100.0f
                || nearPos[i].z >  100.0f
                || nearPos[i].z < -100.0f
                || farPos[i].x >  100.0f
                || farPos[i].x < -100.0f
                || farPos[i].y >  100.0f
                || farPos[i].y < -100.0f
                || farPos[i].z >  100.0f
                || farPos[i].z < -100.0f)
            {
                continue;
            }

            newNear.push_back(nearPos[i]);
            newFar.push_back(farPos[i]);
        }

        nearPos = newNear;
        farPos = newFar;
    }

    void ProcessCalibration(XMFLOAT3& nearCenter, XMFLOAT3& nearLeft, XMFLOAT3& nearRight, XMFLOAT3& nearTop, XMFLOAT3& nearBottom,
        XMFLOAT3& farCenter, XMFLOAT3& farLeft, XMFLOAT3& farRight, XMFLOAT3& farTop, XMFLOAT3& farBottom,
        std::vector<XMFLOAT3>& nearAllPoints, std::vector<XMFLOAT3>& nearTopRow, std::vector<XMFLOAT3>& nearBtmRow, std::vector<XMFLOAT3>& nearLeftCol, std::vector<XMFLOAT3>& nearRightCol,
        std::vector<XMFLOAT3>& farAllPoints, std::vector<XMFLOAT3>& farTopRow, std::vector<XMFLOAT3>& farBtmRow, std::vector<XMFLOAT3>& farLeftCol, std::vector<XMFLOAT3>& farRightCol)
    {
        // clean up
        CleanupPairs(nearAllPoints, farAllPoints);
        CleanupPairs(nearTopRow,    farTopRow);
        CleanupPairs(nearBtmRow,    farBtmRow);
        CleanupPairs(nearLeftCol,   farLeftCol);
        CleanupPairs(nearRightCol,  farRightCol);

        XMVECTOR result = AJMathHelper::GetCentroidAtZ(0.0f, farTop, nearTop, farBottom, nearBottom, farLeft, nearLeft,
            farRight, nearRight, farCenter, nearCenter);

        XMFLOAT3 center;
        XMStoreFloat3(&center, result);

        XMVECTOR newResult = AJMathHelper::RemoveOutliersDoublePlaneCalibration(center,
            nearAllPoints, nearTopRow, nearBtmRow, nearLeftCol, nearRightCol,
            farAllPoints, farTopRow, farBtmRow, farLeftCol, farRightCol);

        float zValue = AJMathHelper::GetZValue(nearAllPoints, farAllPoints);

        std::vector<XMFLOAT3> extendedVectors = AJMathHelper::ExtendVectorsToZ(nearAllPoints, farAllPoints, zValue);
        XMFLOAT3 extendedVectorsCentroid = AJMathHelper::GetCentroidOfVectors(extendedVectors);

        XMVectorSetZ(newResult, extendedVectorsCentroid.z);

        // camera
        XMFLOAT3 camCenter;
        XMStoreFloat3(&camCenter, newResult);
        XMFLOAT3 camLookAt = AJMathHelper::GetAverage(AJMathHelper::MergeVector(farAllPoints, nearAllPoints));
        XMFLOAT3 camLookUp = AJMathHelper::GetAverage(farTopRow);
        XMFLOAT3 camLookDown = AJMathHelper::GetAverage(farBtmRow);
        XMFLOAT3 camLookLeft = AJMathHelper::GetAverage(farLeftCol);
        XMVECTOR camLeftVec = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&camLookLeft), newResult));
        XMVECTOR camLookVec = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&camLookAt), newResult));
        XMVECTOR camUpVec = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&camLookUp), XMLoadFloat3(&camLookDown)));
        XMVECTOR camLookRight = XMVector3Cross(camLookVec, camUpVec);
        camUpVec = XMVector3Cross(camLookRight, camLookVec);
        XMVECTOR camFOVVecX = XMVector3AngleBetweenVectors(camLeftVec, camLookVec);
        XMVECTOR camFOVVecY = XMVector3AngleBetweenVectors(XMVectorSubtract(XMLoadFloat3(&camLookUp), newResult),
            XMVectorSubtract(XMLoadFloat3(&camLookDown), newResult));

        XMVECTOR camLookAtVec = XMLoadFloat3(&camLookAt);
        //XMVectorSetX(newResult,    -XMVectorGetX(newResult));
        //XMVectorSetX(camLookAtVec, -XMVectorGetX(camLookAtVec));

        GFXCamera* camera = (GFXCamera*)GFXManager::GetInstance()->GetMainCamera();
        camera->SetFOV(XMVectorGetX(camFOVVecY) / 0.8f);
        camera->SetCameraPosition(newResult);
        camera->SetCameraLookAt(camLookAtVec);
        camera->SetCameraUpVector(camUpVec);

        cv::FileStorage newFile("../Resource/CameraInfo.yml", cv::FileStorage::WRITE);
        if (newFile.isOpened())
        {
            newFile << "positionx" << XMVectorGetX(newResult);
            newFile << "positiony" << XMVectorGetY(newResult);
            newFile << "positionz" << XMVectorGetZ(newResult);

            newFile << "lookatx" << XMVectorGetX(camLookAtVec);
            newFile << "lookaty" << XMVectorGetY(camLookAtVec);
            newFile << "lookatz" << XMVectorGetZ(camLookAtVec);

            newFile << "camupx" << XMVectorGetX(camUpVec);
            newFile << "camupy" << XMVectorGetY(camUpVec);
            newFile << "camupz" << XMVectorGetZ(camUpVec);

            newFile << "camFOV" << XMVectorGetX(camFOVVecY) / 0.8f;
        }
    }

    void SaveRawData(const char* path, cv::Mat& data)
    {
        cv::FileStorage newFile(path, cv::FileStorage::WRITE);
        newFile << "IMGDATA" << data;
        newFile.release();
    }

    void LoadRawData(const char* path, cv::Mat& data)
    {
        cv::FileStorage readFile(path, cv::FileStorage::READ);
        readFile["IMGDATA"] >> data;
        readFile.release();
    }
}
