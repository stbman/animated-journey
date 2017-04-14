//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJFaceProjectionState.h"
#include "../Source/AJFactoryManager.h"
#include "../Source/AJObject.h"
#include "../Source/gfxHelper.h"
#include "../Source/AJKeyboardManager.h"
#include "../Source/AJColorCalibrationState.h"
#include "../Source/AJProjectorCalibrationState.h"

AJFaceProjectionState::AJFaceProjectionState()
{
}

AJFaceProjectionState::~AJFaceProjectionState()
{
}

void AJFaceProjectionState::OnEnter()
{
    m_ObjList.reserve(7);

    m_ObjList.push_back(AJFactory::CreateFaceObject(0));
    m_ObjList.push_back(AJFactory::CreateFaceObject(1));
    m_ObjList.push_back(AJFactory::CreateFaceObject(2));
    m_ObjList.push_back(AJFactory::CreateFaceObject(3));
    m_ObjList.push_back(AJFactory::CreateFaceObject(4));
    m_ObjList.push_back(AJFactory::CreateFaceObject(5));

}

void AJFaceProjectionState::OnExit()
{
    for each(AJObject* obj in m_ObjList)
    {
        delete obj;
    }

    m_ObjList.clear();
}

void AJFaceProjectionState::Update(float dt)
{
    dt;
    
    if (AJKeyboardManager::GetInstance()->Triggered(AJKeyboardManager::Key::KEY_P))
    {
        AJProjectorCalibrationState* projectorCalibrationState = new AJProjectorCalibrationState();
        AJFlowManager::GetInstance()->ReplaceState(projectorCalibrationState);
    }
    if (AJKeyboardManager::GetInstance()->Triggered(AJKeyboardManager::Key::KEY_C))
    {
        AJColorCalibrationState* colorCalibrationState = new AJColorCalibrationState();
        AJFlowManager::GetInstance()->ReplaceState(colorCalibrationState);
    }
}
