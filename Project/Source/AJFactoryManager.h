//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_FACTORY_MANAGER_H
#define AJ_FACTORY_MANAGER_H
//---------------------------------------------------------------------------

#include <string>
#include <vector>
#include "../Source/AJPointPairGenerator.h"
#include "../Source/AJKinectHelper.h"

class AJObject;
class GFXComponent;
class GFXUIComponent;

namespace AJFactory
{
    AJObject* CreateFaceObject(int index);
    AJObject* CreatePointPairObject(std::vector<AJPointPairGenerator::PointPair>& vectorList, cv::Mat& baseColorFrame);
    GFXUIComponent* CreateQuadComponent(float left, float right, float bottom, float top);
    AJObject* CreateUIObject(std::wstring texturepath);
    AJObject* CreateUIObject(std::wstring texturepath, float normalizedScale);
    AJObject* CreateUIObject(std::wstring texturepath, float left, float right, float bottom, float top);
}

#endif
