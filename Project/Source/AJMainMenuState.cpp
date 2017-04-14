//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJMainMenuState.h"
#include "../Source/AJFactoryManager.h"
#include "../Source/AJObject.h"
#include "../Source/AJKeyboardManager.h"
#include "../Source/AJDemoState.h"
#include "../Source/AJRecordFaceState.h"

AJMainMenuState::AJMainMenuState()
{
}

AJMainMenuState::~AJMainMenuState()
{
}

void TW_CALL SelectionMenu(void * clientData)
{
    int x = 0;
    int button = (int)clientData;
    switch (button)
    {
    case 0:
        AJFlowManager::GetInstance()->ReplaceState(new AJDemoState());
        break;
    case 1:
        x = 2;
        break;
    case 2:
        AJFlowManager::GetInstance()->ReplaceState(new AJRecordFaceState());
        break;
    case 3:
        PostQuitMessage(0);
        break;
    default:
        x = -1;
        break;
    }
}

void AJMainMenuState::OnEnter()
{
    m_ObjList.reserve(7);

    m_ObjList.push_back(AJFactory::CreateFaceObject(0));
    m_ObjList.push_back(AJFactory::CreateFaceObject(1));
    m_ObjList.push_back(AJFactory::CreateFaceObject(2));
    m_ObjList.push_back(AJFactory::CreateFaceObject(3));
    m_ObjList.push_back(AJFactory::CreateFaceObject(4));
    m_ObjList.push_back(AJFactory::CreateFaceObject(5));


    m_SelectionMenu = TwNewBar("Selection Menu");

    int barSize[2] = { 224, 320 };
    TwSetParam(m_SelectionMenu, NULL, "size", TW_PARAM_INT32, 2, barSize);
    
    TwAddButton(m_SelectionMenu, "SelectionMenu0", SelectionMenu, (void*)0, " label='Load Demo' ");
    TwAddButton(m_SelectionMenu, "SelectionMenu1", SelectionMenu, (void*)1, " label='Setup Target' ");
    TwAddButton(m_SelectionMenu, "SelectionMenu2", SelectionMenu, (void*)2, " label='Record Source' ");
    TwAddButton(m_SelectionMenu, "SelectionMenu3", SelectionMenu, (void*)3, " label='Exit' ");

    TwSetParam(m_SelectionMenu, NULL, "visible", TW_PARAM_CSTRING, 1, "true");
}

void AJMainMenuState::OnExit()
{
    for each(AJObject* obj in m_ObjList)
    {
        delete obj;
    }

    m_ObjList.clear();

    TwDeleteBar(m_SelectionMenu);
    m_SelectionMenu = nullptr;
}

void AJMainMenuState::Update(float dt)
{
    dt;
}
