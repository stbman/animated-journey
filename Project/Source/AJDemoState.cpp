//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJDemoState.h"
#include "../Source/AJMainMenuState.h"
#include "../Source/AJFactoryManager.h"
#include "../Source/AJObject.h"
#include "../Source/AJKeyboardManager.h"

AJDemoState::AJDemoState()
{
}

AJDemoState::~AJDemoState()
{
}

void TW_CALL DemoSelectionMenuBackToMainMenu(void * clientData)
{
    AJFlowManager::GetInstance()->ReplaceState(new AJMainMenuState());
}

void TW_CALL DemoSelectionMenuHideMenu(void * clientData)
{
    AJDemoState* demoState = (AJDemoState*)clientData;
    demoState->m_ShowDemoMenu = false;
    TwSetParam(demoState->m_DemoMenu, NULL, "visible", TW_PARAM_CSTRING, 1, "false");
}

void TW_CALL DemoSelectionMenu(void *)
{
    OPENFILENAME fileName;

    TCHAR testPath[100];

    ZeroMemory(&fileName, sizeof(fileName));

    fileName.lStructSize = sizeof(fileName);
    fileName.hwndOwner = 0;
    fileName.lpstrDefExt = 0;
    fileName.lpstrFile = (TCHAR*)&testPath;
    fileName.lpstrFile[0] = '\0';
    fileName.nMaxFile = 100;
    fileName.lpstrFilter = 0;
    fileName.nFilterIndex = 0;
    fileName.lpstrInitialDir = 0;
    fileName.lpstrTitle = 0;
    fileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    //LPOPENFILENAME fileName;
    if (GetOpenFileName(&fileName))
    {
        int x = 0;
    }

    DWORD result = CommDlgExtendedError();
    int x = 0;
}

void AJDemoState::OnEnter()
{
    m_ObjList.reserve(6);

    m_ObjList.push_back(AJFactory::CreateFaceObject(0));
    m_ObjList.push_back(AJFactory::CreateFaceObject(1));
    m_ObjList.push_back(AJFactory::CreateFaceObject(2));
    m_ObjList.push_back(AJFactory::CreateFaceObject(3));
    m_ObjList.push_back(AJFactory::CreateFaceObject(4));
    m_ObjList.push_back(AJFactory::CreateFaceObject(5));


    m_DemoMenu = TwNewBar("Demo Menu");

    int barSize[2] = { 224, 320 };
    TwSetParam(m_DemoMenu, NULL, "size", TW_PARAM_INT32, 2, barSize);
    
    TwAddButton(m_DemoMenu, "SelectionMenu0", DemoSelectionMenu, (void*)this, " label='View Mapping Space' ");
    TwAddButton(m_DemoMenu, "SelectionMenu1", DemoSelectionMenu, (void*)this, " label='Edit Mapping Space' ");
    TwAddButton(m_DemoMenu, "SelectionMenu2", DemoSelectionMenuHideMenu, (void*)this, " label='Hide Menu' ");
    TwAddButton(m_DemoMenu, "SelectionMenu3", DemoSelectionMenuBackToMainMenu, (void*)this, " label='Back to Main Menu' ");

}

void AJDemoState::OnExit()
{
    for each(AJObject* obj in m_ObjList)
    {
        delete obj;
    }

    m_ObjList.clear();

    TwDeleteBar(m_DemoMenu);
    m_DemoMenu = nullptr;
}

void AJDemoState::Update(float dt)
{
    dt;

    if (AJKeyboardManager::GetInstance()->Triggered(AJKeyboardManager::KEY_M))
    {
        m_ShowDemoMenu = !m_ShowDemoMenu;
        if (m_ShowDemoMenu)
        {
            TwSetParam(m_DemoMenu, NULL, "visible", TW_PARAM_CSTRING, 1, "true");
        }
        else
        {
            TwSetParam(m_DemoMenu, NULL, "visible", TW_PARAM_CSTRING, 1, "false");
        }
    }

    bool hasFace = false;
    for each(AJObject* iter in m_ObjList)
    {
        AJFaceComponent* faceComp = iter->FindComponent<AJFaceComponent>();
        if (faceComp->IsTracked())
        {
            hasFace = true;
        }
    }
}
