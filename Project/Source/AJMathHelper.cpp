//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJMathHelper.h"
#include <math.h>
#include <numeric>

namespace AJMathHelper
{
    float DegreeToRadian(float degree)
    {
        return degree * (XM_PI / 180.0f);
    }

    float RadianToDegree(float radian)
    {
        return radian * (180.0f / XM_PI);
    }

    float Lerp(float input, float min, float max)
    {
        return min + (input * (max - min));
    }

    float Clamp(float input, float min, float max)
    {
        return (input >= max) ? max : ( (input <= min) ? min : input);
    }

    float Cursorize(float input, float min, float max)
    {
        float delta = max - min;
        if (abs(delta) <= 0.0001f)
        {
            return min;
        }
        return (input - min) / delta;
    }

    float CursorizeAndClamp(float input, float min, float max)
    {
        return Clamp(Cursorize(input, min, max), min, max);
    }

    float GetLength(XMFLOAT3& a, XMFLOAT3& b)
    {
        XMVECTOR aVec = XMLoadFloat3(&a);
        XMVECTOR bVec  = XMLoadFloat3(&b);
        XMVECTOR delta = XMVectorSubtract(aVec, bVec);
        XMVECTOR distanceVec = XMVector3Length(delta);

        return XMVectorGetX(distanceVec);
    }

    float GetMean(std::vector<float> values)
    {
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / values.size();

        return (float)mean;
    }

    float GetStdDeviation(std::vector<float> values, float mean)
    {
        double sqSum = std::inner_product(values.begin(), values.end(), values.begin(), 0.0);
        double stdDev = std::sqrt(sqSum / values.size() - mean * mean);

        return (float)stdDev;
    }

    float GetStdDeviation(std::vector<float> values)
    {
        float mean = GetMean(values);

        double sqSum = std::inner_product(values.begin(), values.end(), values.begin(), 0.0);
        double stdDev = std::sqrt(sqSum / values.size() - mean * mean);

        return (float)stdDev;
    }

    float GetFirstDeviation(std::vector<float> values)
    {
        float mean = GetMean(values);
        float stdDev = GetStdDeviation(values, mean);

        float distanceThreshold = mean + 2 * stdDev;

        return distanceThreshold;
    }

    std::vector<XMFLOAT3> MergeVector(std::vector<XMFLOAT3>& front, std::vector<XMFLOAT3>& back)
    {
        std::vector<XMFLOAT3> result = front;
        size_t backCount = back.size();
        for (size_t i = 0; i < backCount; ++i)
        {
            result.push_back(back[i]);
        }

        return result;
    }

    XMFLOAT3 GetAverage(std::vector<XMFLOAT3>& pointVector)
    {
        XMFLOAT3 result;
        XMVECTOR current = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        size_t count = pointVector.size();
        if (count <= 0)
        {
            XMStoreFloat3(&result, current);
            return result;
        }

        for (size_t i = 0; i < count; ++i)
        {
            XMVECTOR iter = XMLoadFloat3(&pointVector[i]);
            current = XMVectorAdd(current, iter);
        }

        float divider = (float)count;
        current = XMVectorDivide(current, XMVectorSet(divider, divider, divider, divider));
        XMStoreFloat3(&result, current);
        return result;
    }

    XMFLOAT3 ExtendToZ(XMFLOAT3& farPoint, XMFLOAT3& nearPoint, float z)
    {
        XMVECTOR farVec  = XMLoadFloat3(&farPoint);
        XMVECTOR nearVec = XMLoadFloat3(&nearPoint);
        XMVECTOR delta   = XMVectorSubtract(nearVec, farVec);
        XMVECTOR backVec = delta   / XMVectorGetZ(delta);
        XMVECTOR extend  = backVec * (farPoint.z - z);
        XMVECTOR zeroZ   = XMVectorSubtract(farVec, extend);

        XMFLOAT3 result;
        XMStoreFloat3(&result, zeroZ);

        return result;
    }

    XMVECTOR GetCentroidAtZ(float z, XMFLOAT3 farTop, XMFLOAT3 nearTop, XMFLOAT3 farBtm, XMFLOAT3 nearBtm,
        XMFLOAT3 farLeft, XMFLOAT3 nearLeft, XMFLOAT3 farRight, XMFLOAT3 nearRight,
        XMFLOAT3 farCenter, XMFLOAT3 nearCenter)
    {
        XMFLOAT3 center = ExtendToZ(farCenter, nearCenter, z);
        XMFLOAT3 left   = ExtendToZ(farLeft, nearLeft, z);
        XMFLOAT3 right  = ExtendToZ(farRight, nearRight, z);
        XMFLOAT3 top    = ExtendToZ(farTop, nearTop, z);
        XMFLOAT3 bottom = ExtendToZ(farBtm, nearBtm, z);

        XMVECTOR result = XMLoadFloat3(&center)
                        + XMLoadFloat3(&left)
                        + XMLoadFloat3(&right)
                        + XMLoadFloat3(&top)
                        + XMLoadFloat3(&bottom);
        result /= 5;

        return result;
    }

    void PrunePoints(XMFLOAT3& center, std::vector<XMFLOAT3>& farPoints, std::vector<XMFLOAT3>& nearPoints)
    {
        std::vector<float> distancesFromCenter;
        size_t count = farPoints.size();

        for (size_t i = 0; i < count; ++i)
        {
            XMFLOAT3 nearPoint = nearPoints.at(i);
            XMFLOAT3 farPoint = farPoints.at(i);
            XMFLOAT3 resultPoint = AJMathHelper::ExtendToZ(farPoint, nearPoint, 0.0f);
            float distance = GetLength(center, resultPoint);
            distancesFromCenter.push_back(distance);
        }

        float distanceThreshold = GetFirstDeviation(distancesFromCenter);

        std::vector<XMFLOAT3> nearCleanedUp;
        std::vector<XMFLOAT3> farCleanedUp;

        for (int i = 0; i < count; ++i)
        {
            if (distancesFromCenter.at(i) < distanceThreshold)
            {
                farCleanedUp.push_back(farPoints[i]);
                nearCleanedUp.push_back(nearPoints[i]);
            }
        }

        farPoints  = farCleanedUp;
        nearPoints = nearCleanedUp;
    }

    XMVECTOR RemoveOutliersDoublePlaneCalibration(XMFLOAT3& center, std::vector<XMFLOAT3>& nearCenterAll,
        std::vector<XMFLOAT3>& nearTopRow, std::vector<XMFLOAT3>& nearBtmRow,
        std::vector<XMFLOAT3>& nearLeftCol, std::vector<XMFLOAT3>& nearRightCol, std::vector<XMFLOAT3>& farCenterAll,
        std::vector<XMFLOAT3>& farTopRow, std::vector<XMFLOAT3>& farBtmRow,
        std::vector<XMFLOAT3>& farLeftCol, std::vector<XMFLOAT3>& farRightCol)
    {
        XMFLOAT3 farTop, nearTop, farBtm, nearBtm, farLeft, nearLeft, farRight, nearRight, farCenter, nearCenter;

        AJMathHelper::PrunePoints(center, farTopRow,    nearTopRow);
        AJMathHelper::PrunePoints(center, farBtmRow,    nearBtmRow);
        AJMathHelper::PrunePoints(center, farLeftCol,   nearLeftCol);
        AJMathHelper::PrunePoints(center, farRightCol,  nearRightCol);
        AJMathHelper::PrunePoints(center, farCenterAll, nearCenterAll);

        farLeft    = AJMathHelper::GetAverage(farLeftCol);
        nearLeft   = AJMathHelper::GetAverage(nearLeftCol);
        farRight   = AJMathHelper::GetAverage(farRightCol);
        nearRight  = AJMathHelper::GetAverage(nearRightCol);
        farBtm     = AJMathHelper::GetAverage(farBtmRow);
        nearBtm    = AJMathHelper::GetAverage(nearBtmRow);
        farTop     = AJMathHelper::GetAverage(farTopRow);
        nearTop    = AJMathHelper::GetAverage(nearTopRow);
        farCenter  = AJMathHelper::GetAverage(farCenterAll);
        nearCenter = AJMathHelper::GetAverage(nearCenterAll);


        XMVECTOR result = AJMathHelper::GetCentroidAtZ(0.0f, farTop, nearTop, farBtm, nearBtm, farLeft, nearLeft,
            farRight, nearRight, farCenter, nearCenter);

        return result;
    }

    float GetZValue(std::vector<XMFLOAT3>& nearAllPoints, std::vector<XMFLOAT3>& farAllPoints)
    {

        float zVal = GetFarthestZValue(farAllPoints);
        float lowerZBound = -zVal;

        float prevStdDev = GetStdDevOfPointsToPlane(zVal, nearAllPoints, farAllPoints);
        float prevGradient = -1.0;

        while (zVal > lowerZBound)
        {
            zVal -= 0.03;
            float currStdDev = GetStdDevOfPointsToPlane(zVal, nearAllPoints, farAllPoints);
            float delta = prevStdDev - currStdDev;
            prevStdDev = currStdDev;


            float currGradient;
            delta > 0 ? currGradient = -1.0f : currGradient = 1.0f;

            if (currGradient != prevGradient)
            {
                break;
            }
        }

        return zVal;
    }

    float GetFarthestZValue(std::vector<XMFLOAT3>& farAllPoints)
    {
        float zVal = -FLT_MAX;

        for (int i = 0; i < farAllPoints.size(); ++i)
        {
            XMFLOAT3 currPoint = farAllPoints[i];
            if (currPoint.z > zVal)
            {
                zVal = currPoint.z;
            }
        }

        return zVal;
    }

    std::vector<XMFLOAT3> ExtendVectorsToZ(std::vector<XMFLOAT3>& nearAllPoints, std::vector<XMFLOAT3>& farAllPoints, float z)
    {
        int count = nearAllPoints.size();
        std::vector<XMFLOAT3> extendedVectors;

        for (int i = 0; i < count; ++i)
        {
            XMFLOAT3 nearPoint = nearAllPoints[i];
            XMFLOAT3 farPoint = farAllPoints[i];
            XMFLOAT3 extendedVec = ExtendToZ(farPoint, nearPoint, z);
            extendedVectors.push_back(extendedVec);
        }

        return extendedVectors;
    }

    XMFLOAT3 GetCentroidOfVectors(std::vector<XMFLOAT3>& vectors)
    {
        XMVECTOR result = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); 
        int count = vectors.size();

        for (int i = 0; i < count; ++i)
        {
            XMVECTOR currVector = XMLoadFloat3(&vectors[i]);
            result += currVector;
        }

        result /= count;

        XMFLOAT3 centroid;
        XMStoreFloat3(&centroid, result);

        return centroid;
    }

    float GetStdDevOfPointsToPlane(float z, std::vector<XMFLOAT3>& nearAllPoints, std::vector<XMFLOAT3>& farAllPoints)
    {
        int count = nearAllPoints.size();

        std::vector<XMFLOAT3> extendedVectors = ExtendVectorsToZ(nearAllPoints, farAllPoints, z);
        XMFLOAT3 centroid = GetCentroidOfVectors(extendedVectors);

        std::vector<float> distancesFromCenter;
        for (int i = 0; i < count; ++i)
        {
            XMFLOAT3 currentPoint = extendedVectors[i];
            float distance = GetLength(centroid, currentPoint);
            distancesFromCenter.push_back(distance);
        }

        float stdDev = GetStdDeviation(distancesFromCenter);

        return stdDev;
    }

    float CalculateNewZCenter(float upper, float lower)
    {
        return (upper - lower) / 2;
    }

}
