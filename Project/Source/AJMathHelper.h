//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_MATH_HELPER_H
#define AJ_MATH_HELPER_H
//---------------------------------------------------------------------------

#include "../Source/AJMathHeader.h"
#include <vector>

namespace AJMathHelper
{
    float DegreeToRadian(float degree);
    float RadianToDegree(float radian);
    float Lerp(float input, float min, float max);
    float Clamp(float input, float min, float max);
    float Cursorize(float input, float min, float max);
    float CursorizeAndClamp(float input, float min, float max);

    float GetLength(XMFLOAT3& a, XMFLOAT3& b);
    float GetMean(std::vector<float> values);
    float GetStdDeviation(std::vector<float> values, float mean);
    float GetStdDeviation(std::vector<float> values);
    float GetFirstDeviation(std::vector<float> values);
    std::vector<XMFLOAT3> MergeVector(std::vector<XMFLOAT3>& front, std::vector<XMFLOAT3>& back);

    XMFLOAT3 GetAverage(std::vector<XMFLOAT3>& pointVector);
    XMFLOAT3 ExtendToZ(XMFLOAT3& farPoint, XMFLOAT3& nearPoint, float z);
    XMVECTOR GetCentroidAtZ(float z, XMFLOAT3 farTop, XMFLOAT3 nearTop, XMFLOAT3 farBtm, XMFLOAT3 nearBtm,
        XMFLOAT3 farLeft, XMFLOAT3 nearLeft, XMFLOAT3 farRight, XMFLOAT3 nearRight,
        XMFLOAT3 farCenter, XMFLOAT3 nearCenter);
    void PrunePoints(XMFLOAT3& center, std::vector<XMFLOAT3>& farPoints, std::vector<XMFLOAT3>& nearPoints);
    XMVECTOR RemoveOutliersDoublePlaneCalibration(XMFLOAT3& center, std::vector<XMFLOAT3>& nearCenterAll,
        std::vector<XMFLOAT3>& nearTopRow, std::vector<XMFLOAT3>& nearBtmRow,
        std::vector<XMFLOAT3>& nearLeftCol, std::vector<XMFLOAT3>& nearRightCol, std::vector<XMFLOAT3>& farCenterAll,
        std::vector<XMFLOAT3>& farTopRow, std::vector<XMFLOAT3>& farBtmRow,
        std::vector<XMFLOAT3>& farLeftCol, std::vector<XMFLOAT3>& farRightCol);

    float GetZValue(std::vector<XMFLOAT3>& nearAllPoints, std::vector<XMFLOAT3>& farAllPoints);
    float GetFarthestZValue(std::vector<XMFLOAT3>& farAllPoints);
    std::vector<XMFLOAT3> ExtendVectorsToZ(std::vector<XMFLOAT3>& nearAllPoints, std::vector<XMFLOAT3>& farAllPoints, float z);
    XMFLOAT3 GetCentroidOfVectors(std::vector<XMFLOAT3>& vectors);
    float GetStdDevOfPointsToPlane(float z, std::vector<XMFLOAT3>& nearAllPoints, std::vector<XMFLOAT3>& farAllPoints);
    float CalculateNewZCenter(float upper, float lower);
}

#endif
