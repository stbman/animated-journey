//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJControlFlowState.h"
#include "../Source/AJMainMenuState.h"
#include "../Source/gfxcamera.h"
#include "../Source/gfxorthocamera.h"
#include "../Source/AJKinectHelper.h"

AJControlFlowState::AJControlFlowState()
{
}

AJControlFlowState::~AJControlFlowState()
{
}

void AJControlFlowState::OnEnter()
{
    GFXManager* gfxManager = GFXManager::GetInstance();
    m_MainCamera = new GFXCamera();
    m_MainCamera->Init();
    gfxManager->SetMainCamera(m_MainCamera);

    m_UICamera = new GFXOrthoCamera();
    m_UICamera->Init();
    gfxManager->SetUICamera(m_UICamera);

    gfxManager->SetActiveCamera(true);

    AJMainMenuState* mainMenuState = new AJMainMenuState();
    AJFlowManager::GetInstance()->PushState(mainMenuState);
}

void AJControlFlowState::OnExit()
{
    GFXManager* gfxManager = GFXManager::GetInstance();
    gfxManager->SetMainCamera(nullptr);
    gfxManager->SetUICamera(nullptr);

    SafeDelete(m_MainCamera);
    SafeDelete(m_UICamera);
}

void AJControlFlowState::Update(float dt)
{
    dt;
}
