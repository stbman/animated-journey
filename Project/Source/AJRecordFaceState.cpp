//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJRecordFaceState.h"
#include "../Source/AJMainMenuState.h"
#include "../Source/AJFactoryManager.h"
#include "../Source/AJObject.h"
#include "../Source/AJKeyboardManager.h"

AJRecordFaceState::AJRecordFaceState()
{
}

AJRecordFaceState::~AJRecordFaceState()
{
}

void TW_CALL RecordFaceMenuBackToMainMenu(void * clientData)
{
    AJFlowManager::GetInstance()->ReplaceState(new AJMainMenuState());
}

void TW_CALL RecordFaceMenuHideMenu(void * clientData)
{
    AJRecordFaceState* demoState = (AJRecordFaceState*)clientData;
    demoState->m_ShowDemoMenu = false;
    TwSetParam(demoState->m_DemoMenu, NULL, "visible", TW_PARAM_CSTRING, 1, "false");
}

void TW_CALL RecordFaceMenu(void *)
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

void AJRecordFaceState::OnEnter()
{
    m_ObjList.reserve(6);

    m_ObjList.push_back(AJFactory::CreateFaceObject(0));
    m_ObjList.push_back(AJFactory::CreateFaceObject(1));
    m_ObjList.push_back(AJFactory::CreateFaceObject(2));
    m_ObjList.push_back(AJFactory::CreateFaceObject(3));
    m_ObjList.push_back(AJFactory::CreateFaceObject(4));
    m_ObjList.push_back(AJFactory::CreateFaceObject(5));


    m_DemoMenu = TwNewBar("Recorder Menu");

    int barSize[2] = { 224, 320 };
    TwSetParam(m_DemoMenu, NULL, "size", TW_PARAM_INT32, 2, barSize);
    
    TwAddButton(m_DemoMenu, "SelectionMenu1", RecordFaceMenuBackToMainMenu, (void*)this, " label='Back to Main Menu' ");

    m_StartRecording = AJFactory::CreateUIObject(L"../Resource/startrecording.dds", 0.2f);
    DirectX::XMVECTOR pos = GFXHelper::ConvertNormalizedToScreenSpace(0.9f, 0.9f, 0.0f);
    m_StartRecording->SetGlobalPosition(pos);

    m_StopRecording = AJFactory::CreateUIObject(L"../Resource/stoprecording.dds", 0.2f);
    m_StopRecording->SetGlobalPosition(pos);
    GFXHelper::SetVisibility(m_StopRecording, false);
}

void AJRecordFaceState::OnExit()
{
    for each(AJObject* obj in m_ObjList)
    {
        delete obj;
    }

    m_ObjList.clear();

    TwDeleteBar(m_DemoMenu);
    m_DemoMenu = nullptr;
}

void AJRecordFaceState::Update(float dt)
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

    if (AJKeyboardManager::GetInstance()->Triggered(AJKeyboardManager::KEY_R))
    {
        if (AJKinect::GetInstance()->m_IsRecording)
        {
            // stop
            GFXHelper::SetVisibility(m_StartRecording, true);
            GFXHelper::SetVisibility(m_StopRecording, false);

            // stop recording
            AJKinect::GetInstance()->m_AudioRecorder.Close();
            AJKinect::GetInstance()->m_FaceRecorder.Close();
        }
        else
        {
            // start
            GFXHelper::SetVisibility(m_StartRecording, false);
            GFXHelper::SetVisibility(m_StopRecording, true);

            // start recording
            AJKinect::GetInstance()->m_FaceRecorder.m_ObjList = m_ObjList;
            AJKinect::GetInstance()->m_AudioRecorder.Init(L"../Saved/recording.wav", 1, 16000, 16);
            AJKinect::GetInstance()->m_FaceRecorder.Init(L"../Saved/face.apf");
        }
        AJKinect::GetInstance()->m_IsRecording = !AJKinect::GetInstance()->m_IsRecording;
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

    // grab active face
    AJFaceComponent* activeFace = nullptr;
    size_t objSize = m_ObjList.size();
    for (size_t i = 0; i < objSize; ++i)
    {
        AJFaceComponent* faceComp = m_ObjList[i]->FindComponent<AJFaceComponent>();
        if (!faceComp)
            continue;

        if (faceComp->GetActive())
        {
            activeFace = faceComp;
            break;
        }
    }


        // start audio
        // safe frames using fps
}
