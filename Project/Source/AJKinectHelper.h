//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_KINECT_HELPER_H
#define AJ_KINECT_HELPER_H
//---------------------------------------------------------------------------

#include "../Source/AJKinect.h"
#include "../Source/AJMathHelper.h"
#include "../Source/AJPointPairGenerator.h"

namespace AJKinectHelper
{
    bool IsValidPoint(cv::Mat depth, UINT x, UINT y);
    bool IsValidPoint(cv::Mat depth, XMFLOAT2& center);
    float GetDepth(cv::Mat depth, UINT x, UINT y);
    bool GetCameraPoint(XMFLOAT3& result, cv::Mat cameraPoint, UINT x, UINT y);
    bool GetCameraPoint(XMFLOAT3& result, cv::Mat cameraPoint, XMFLOAT2& center);
    bool GetImageColorValue(XMFLOAT3& result, cv::Mat cameraPoint, UINT x, UINT y);
    bool GetImageColorValue(XMFLOAT3& result, cv::Mat cameraPoint, XMFLOAT2& center);
    bool ProcessImage(cv::Mat& base, cv::Mat& raw, cv::Mat& space, XMFLOAT3& position);
    void PerformCalibrationOnDelta(std::vector<AJPointPairGenerator::PointPair>& first, std::vector<AJPointPairGenerator::PointPair>& second);
    void GrabPointsFromPPGen(std::vector<AJPointPairGenerator::PointPair>& data, XMFLOAT3& center, XMFLOAT3& left, XMFLOAT3& right,
        XMFLOAT3& top, XMFLOAT3& bottom, std::vector<XMFLOAT3> &allPoints, std::vector<XMFLOAT3> &topRow,
        std::vector<XMFLOAT3> &btmRow, std::vector<XMFLOAT3> &leftCol, std::vector<XMFLOAT3> &rightCol);
    void CleanupPairs(std::vector<XMFLOAT3>& nearPos, std::vector<XMFLOAT3>& farPos);
    void ProcessCalibration(XMFLOAT3& nearCenter, XMFLOAT3& nearLeft, XMFLOAT3& nearRight, XMFLOAT3& nearTop, XMFLOAT3& nearBottom,
                            XMFLOAT3& farCenter,  XMFLOAT3& farLeft,  XMFLOAT3& farRight,  XMFLOAT3& farTop,  XMFLOAT3& farBottom,
                            std::vector<XMFLOAT3>& nearAllPoints, std::vector<XMFLOAT3>& nearTopRow, std::vector<XMFLOAT3>& nearBtmRow, std::vector<XMFLOAT3>& nearLeftCol, std::vector<XMFLOAT3>& nearRightCol,
                            std::vector<XMFLOAT3>& farAllPoints,  std::vector<XMFLOAT3>& farTopRow,  std::vector<XMFLOAT3>& farBtmRow,  std::vector<XMFLOAT3>& farLeftCol,  std::vector<XMFLOAT3>& farRightCol);

    void SaveRawData(const char* path, cv::Mat& data);
    void LoadRawData(const char* path, cv::Mat& data);
}

#endif
