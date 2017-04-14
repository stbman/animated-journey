//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_POINT_PAIR_COMPONENT_H
#define AJ_POINT_PAIR_COMPONENT_H
//---------------------------------------------------------------------------

#include "../Source/AJBaseComponent.h"
#include "../Source/AJPointPairGenerator.h"
#include "../Source/AJStateManager.h"
#include "../Source/AJKinectHelper.h"
#include <vector>

class AJPointPairComponent : public AJBaseComponent
{
private:
    AJStateManager m_Logic;
    unsigned int m_SearchCount = 0;
    unsigned int m_CurrentIter = 0;

public:
    enum Status
    {
        Inprogress,
        Done
    };

    std::vector<AJPointPairGenerator::PointPair> m_PointPair;
    cv::Mat m_BaseColorFrame;

    AJPointPairComponent(std::vector<AJPointPairGenerator::PointPair>& vectorList, cv::Mat& baseColorFrame);
    ~AJPointPairComponent();


    void virtual Update();
    Status GetStatus();
};

#endif