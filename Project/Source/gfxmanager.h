//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef AJ_GFXMANAGER_H
#define AJ_GFXMANAGER_H
//---------------------------------------------------------------------------

#include "../Source/predefine.h"
#include <DirectXMath.h>

class GFXBaseCamera;

class GFXManager
{
private:
    GFXManager();
    ~GFXManager();

    LONG m_StartWindowWidth;
    LONG m_StartWindowHeight;
    LONG m_WindowWidth;
    LONG m_WindowHeight;
    float m_ClearColor[4];

    HINSTANCE                 m_hInst                          = nullptr;
    HWND                      m_hWnd                           = nullptr;
    D3D_DRIVER_TYPE           m_driverType                     = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL         m_featureLevel                   = D3D_FEATURE_LEVEL_11_0;
    ID3D11Device*             m_pd3dDevice                     = nullptr;
    ID3D11Device1*            m_pd3dDevice1                    = nullptr;
    ID3D11DeviceContext*      m_pImmediateContext              = nullptr;
    ID3D11DeviceContext1*     m_pImmediateContext1             = nullptr;
    IDXGISwapChain*           m_pSwapChain                     = nullptr;
    IDXGISwapChain1*          m_pSwapChain1                    = nullptr;
    ID3D11RenderTargetView*   m_pRenderTargetView              = nullptr;
    ID3D11Texture2D*          m_pDepthStencil                  = nullptr;
    ID3D11DepthStencilView*   m_pDepthStencilView              = nullptr;
    ID3D11Buffer*             m_pCBViewMatrix                  = nullptr;
    ID3D11Buffer*             m_pCBProjectionMatrix            = nullptr;
    ID3D11DepthStencilState*  m_DepthStencilStateDepthEnabled  = nullptr;
    ID3D11DepthStencilState*  m_DepthStencilStateDepthDisabled = nullptr;
    
    // state memento
    bool m_DepthEnabled = true;

    // objects
    GFXBaseCamera* m_ActiveCamera = nullptr;
    GFXBaseCamera* m_MainCamera   = nullptr;
    GFXBaseCamera* m_UICamera     = nullptr;

    /// Helper for compiling shaders with D3DCompile
    /// With VS 11, we could load up prebuilt .cso files instead...
public:
    static GFXManager* GetInstance();
    LONG GetStartWindowWidth()  { return m_StartWindowWidth; }
    LONG GetStartWindowHeight() { return m_StartWindowHeight; }
    LONG GetWindowWidth()  { return m_WindowWidth; }
    LONG GetWindowHeight() { return m_WindowHeight; }
    ID3D11Device*        GetD3DDevice();
    ID3D11DeviceContext* GetD3DDeviceContext();
    HWND GetHWND() { return m_hWnd; }
    void SetClearRed(float r);
    void SetClearGreen(float g);
    void SetClearBlue(float b);
    void SetClearAlpha(float a);
    float GetClearRed();
    float GetClearGreen();
    float GetClearBlue();
    float GetClearAlpha();
    
    bool GetDepthEnabled();
    void SetDepth(bool enabled);

    GFXBaseCamera*       GetActiveCamera();
    GFXBaseCamera*       GetMainCamera();
    GFXBaseCamera*       GetUICamera();
    void SetActiveCamera(bool isMainCamera);
    void SetMainCamera(GFXBaseCamera* camera);
    void SetUICamera(GFXBaseCamera* camera);
    void UpdateCameras();
    
    void SetViewMatrix(const DirectX::XMMATRIX& viewMatrix);
    void SetProjectionMatrix(const DirectX::XMMATRIX& projectMatrix);

    static HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
    // TODO: refactor and make it modular and component based
    // From dx example

    /// Register class and create window
    HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
    /// Create Direct3D device and swap chain
    HRESULT InitDevice();
    /// Clean up the objects created
    void CleanupDevice();
    /// Render a frame
    void StartRender();
    void StartRenderUI();
    void EndRender();



};

#endif