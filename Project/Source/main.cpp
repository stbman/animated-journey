//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------

// shared resources
#include "../Source/predefine.h"

// to be shifted to scene/state manager
#include "../Source/AJObject.h"
//#include "../Source/gfxcomponent.h"
#include "../Source/AJControlFlowState.h"
#include "../Source/AJKeyboardManager.h"

#include "../Source/AJMathHelper.h"
#include "../Source/AJKinectHelper.h"

#include <fstream>
#include <numeric>

#include <fstream>

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    std::ifstream newFile;
    newFile.open("../Saved/6/face.apf", std::ios::binary);
    if (newFile.is_open())
    {
        newFile.seekg(0, std::ios::end);
        UINT32 length = newFile.tellg();
        newFile.seekg(0, std::ios::beg);
        UINT32 frames = length / 1347;
        UINT32 vertexCount = length / sizeof(SimpleVertex);
        SimpleVertex* buffer = new SimpleVertex[vertexCount];
        // read data as a block:
        newFile.read((char*)buffer, length);
        newFile.close();

        std::ofstream cleanFile;
        cleanFile.open("../Saved/6/faceProcessed.apf", std::ios::binary);
        if (cleanFile.is_open())
        {
            for (UINT32 i = 0; i < vertexCount; ++i)
            {
                buffer[i].Pos.x = buffer[HighDetailFacePoints_NoseTip].Pos.x;
                buffer[i].Pos.y = buffer[HighDetailFacePoints_NoseTip].Pos.y;
                buffer[i].Pos.z = buffer[HighDetailFacePoints_NoseTip].Pos.z;
            }
            cleanFile.write((char*)buffer, vertexCount * sizeof(SimpleVertex));
            cleanFile.close();
        }
    }

    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    GFXManager* GFXManagerInstance = GFXManager::GetInstance();
    
    // Initialize DX
    if (FAILED(GFXManagerInstance->InitWindow(hInstance, nCmdShow)))
        return 0;

    if (FAILED(GFXManagerInstance->InitDevice()))
    {
        GFXManagerInstance->CleanupDevice();
        return 0;
    }

    // Initialize Kinect
    if (FAILED(AJKinect::GetInstance()->InitKinect()))
    {
        AJKinect::GetInstance()->CleanupKinect();
        GFXManagerInstance->CleanupDevice();
        return 0;
    }

    // to be made into a scene state manager
    AJTimeManager* timeManager = AJTimeManager::GetInstance();
    timeManager->Start();

    FFObjectManager* objManager = FFObjectManager::GetInstance();

    // end// to be made into a scene state manager

    AJFlowManager* flowManager = AJFlowManager::GetInstance();
    flowManager->PushState(new AJControlFlowState());

    AJKinect* kinectManager = AJKinect::GetInstance();
    
    AJKeyboardManager* keyboardManager = AJKeyboardManager::GetInstance();


    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            timeManager->Update();
            keyboardManager->Update();
            kinectManager->StartFace();
            // Update state with frames per second
            flowManager->Update(timeManager->GetDeltaTime());
            objManager->Update();

            // DX Screen
            GFXManagerInstance->StartRender();
            objManager->Render();
            GFXManagerInstance->StartRenderUI();
            objManager->RenderUI();

            // Put final buffer onto screen
            GFXManagerInstance->EndRender();

            

            // Kinect Screen
            //kinectManager->GetFace();
        }
    }

    // to be made into a scene state manager
    // end// to be made into a scene state manager

    AJKinect::GetInstance()->CleanupKinect();
    flowManager->ClearStates();
    objManager->ClearObjects();
    GFXManagerInstance->CleanupDevice();

    return ( int )msg.wParam;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    if (TwEventWin(hWnd, message, wParam, lParam)) // send event message to AntTweakBar
        return 0;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

            // Note that this tutorial does not handle resizing (WM_SIZE) requests,
            // so we created the window without the resize border.

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
                PostQuitMessage(0);

        case WM_TIMER:
            // Capture all available audio or update audio panel each time a timer fires
            if (wParam == AJKinect::cAudioReadTimerId)
            {
                AJKinect::GetInstance()->GetFace();
                AJKinect::GetInstance()->ProcessCapturing();
            }

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

