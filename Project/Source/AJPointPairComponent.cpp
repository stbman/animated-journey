//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJPointPairComponent.h"
#include "../Source/AJTimeManager.h"
#include "../Source/AJPointPairLogicInit.h"
#include "../Source/AJPointPairLogicCapture.h"

AJPointPairComponent::AJPointPairComponent(std::vector<AJPointPairGenerator::PointPair>& vectorList, cv::Mat& baseColorFrame)
{
    m_PointPair = vectorList;
    baseColorFrame.copyTo(m_BaseColorFrame);
}

AJPointPairComponent::~AJPointPairComponent()
{
}

void AJPointPairComponent::Update()
{
    m_Logic.Update(AJTimeManager::GetInstance()->GetDeltaTime());

    if (GetStatus() == AJPointPairComponent::Status::Done)
    {
        return;
    }

    AJPointPairLogicBaseState* baseState = dynamic_cast<AJPointPairLogicBaseState*>(m_Logic.GetCurrentState());
    if (!baseState)
    {
        baseState = new AJPointPairLogicInit(this, m_PointPair[m_SearchCount].m_Source);
        m_Logic.PushState(baseState);
        m_CurrentIter = m_SearchCount;
    }

    AJPointPairLogicBaseState::Status currentStatus = baseState->GetStatus();
    switch (currentStatus)
    {
    case AJPointPairLogicBaseState::InProgress:
        return;
        break;
    case AJPointPairLogicBaseState::Done:
        if (dynamic_cast<AJPointPairLogicInit*>(baseState))
        {
            AJPointPairLogicCapture* pointPairLogicCapture = new AJPointPairLogicCapture(this);
            m_Logic.ReplaceState(pointPairLogicCapture);
            break;
        }
        {
            AJPointPairLogicCapture* pointPairLogicCapture = dynamic_cast<AJPointPairLogicCapture*>(baseState);
            if (pointPairLogicCapture)
            {
                // grab result
                m_PointPair[m_CurrentIter].m_Result = pointPairLogicCapture->GetResult();
                m_PointPair[m_CurrentIter].m_Found  = true;
            }
        }
    case AJPointPairLogicBaseState::Error:
    default:
        m_Logic.ClearStates();
        ++m_SearchCount;
    }
}

AJPointPairComponent::Status AJPointPairComponent::GetStatus()
{
    if (m_SearchCount >= m_PointPair.size())
    {
        return AJPointPairComponent::Status::Done;
    }
    return AJPointPairComponent::Status::Inprogress;
}