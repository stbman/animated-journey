//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/gfxmanager.h"
#include "../Source/gfxbasecamera.h"
#include <AntTweakBar.h>

GFXManager::GFXManager()
{
    m_StartWindowWidth = 0;// 2560 + 1280;
    m_StartWindowHeight = 0;
    m_WindowWidth       = 1024 + 512;
    m_WindowHeight      = 768 + 384;
}

GFXManager::~GFXManager()
{
}

GFXManager* GFXManager::GetInstance()
{
    static GFXManager m_Instance;
    return &m_Instance;
}

ID3D11Device* GFXManager::GetD3DDevice()
{
    return m_pd3dDevice;
}

ID3D11DeviceContext* GFXManager::GetD3DDeviceContext()
{
    return m_pImmediateContext;
}

void GFXManager::SetClearRed(float r)
{
    m_ClearColor[2] = r;
}

void GFXManager::SetClearGreen(float g)
{
    m_ClearColor[1] = g;
}

void GFXManager::SetClearBlue(float b)
{
    m_ClearColor[0] = b;
}

void GFXManager::SetClearAlpha(float a)
{
    m_ClearColor[3] = a;
}

float GFXManager::GetClearRed()
{
    return m_ClearColor[2];
}

float GFXManager::GetClearGreen()
{
    return m_ClearColor[1];
}

float GFXManager::GetClearBlue()
{
    return m_ClearColor[0];
}

float GFXManager::GetClearAlpha()
{
    return m_ClearColor[3];
}

bool GFXManager::GetDepthEnabled()
{
    return m_DepthEnabled;
}

void GFXManager::SetDepth(bool enabled)
{
    if (m_DepthEnabled != enabled)
    {
        m_DepthEnabled = enabled;

        if (enabled)
        {
            m_pImmediateContext->OMSetDepthStencilState(m_DepthStencilStateDepthEnabled, 0);
        }
        else
        {
            m_pImmediateContext->OMSetDepthStencilState(m_DepthStencilStateDepthDisabled, 0);
        }
    }
}

GFXBaseCamera* GFXManager::GetActiveCamera()
{
    return m_ActiveCamera;
}

GFXBaseCamera* GFXManager::GetMainCamera()
{
    return m_MainCamera;
}

GFXBaseCamera* GFXManager::GetUICamera()
{
    return m_UICamera;
}

void GFXManager::SetActiveCamera(bool isMainCamera)
{
    if (isMainCamera)
    {
        if (m_ActiveCamera != m_MainCamera)
        {
            m_ActiveCamera = m_MainCamera;
            SetViewMatrix(m_ActiveCamera->GetViewMatrix());
            SetProjectionMatrix(m_ActiveCamera->GetProjMatrix());
        }
    }
    else
    {
        if (m_ActiveCamera != m_UICamera)
        {
            m_ActiveCamera = m_UICamera;
            SetViewMatrix(m_ActiveCamera->GetViewMatrix());
            SetProjectionMatrix(m_ActiveCamera->GetProjMatrix());
        }
    }
}

void GFXManager::SetMainCamera(GFXBaseCamera* camera)
{
    m_MainCamera = camera;
}

void GFXManager::SetUICamera(GFXBaseCamera* camera)
{
    m_UICamera = camera;
}

void GFXManager::UpdateCameras()
{
    if (m_MainCamera)
    {
        m_MainCamera->Update();
    }

    if (m_UICamera)
    {
        m_UICamera->Update();
    }
}

void GFXManager::SetViewMatrix(const DirectX::XMMATRIX& viewMatrix)
{
    CBViewMatrix cbViewMatrix;
    cbViewMatrix.mView = XMMatrixTranspose(viewMatrix);
    m_pImmediateContext->UpdateSubresource(m_pCBViewMatrix, 0, nullptr, &cbViewMatrix, 0, 0);
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pCBViewMatrix);
}

void GFXManager::SetProjectionMatrix(const DirectX::XMMATRIX& projectMatrix)
{
    CBProjectionMatrix cbProjectionMatrix;
    cbProjectionMatrix.mProjection = XMMatrixTranspose(projectMatrix);
    m_pImmediateContext->UpdateSubresource(m_pCBProjectionMatrix, 0, nullptr, &cbProjectionMatrix, 0, 0);
    m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_pCBProjectionMatrix);
}

HRESULT GFXManager::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT GFXManager::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"LynnetteFYP";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    m_hInst = hInstance;
    //AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    m_hWnd = CreateWindow(L"LynnetteFYP",
                          L"Lynnette FYP",
                          WS_POPUP,
                          GetStartWindowWidth(),
                          GetStartWindowHeight(),
                          GetWindowWidth(), GetWindowHeight(),
                          nullptr,
                          nullptr,
                          hInstance,
        nullptr);
    if (!m_hWnd)
        return E_FAIL;

    ShowWindow(m_hWnd, nCmdShow);

    return S_OK;
}

HRESULT GFXManager::InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(m_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        m_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);

        if (hr == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // DirectX 11.1 or later
        hr = m_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&m_pd3dDevice1));
        if (SUCCEEDED(hr))
        {
            (void)m_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&m_pImmediateContext1));
        }

        DXGI_SWAP_CHAIN_DESC1 sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd(m_pd3dDevice, m_hWnd, &sd, nullptr, nullptr, &m_pSwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = m_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&m_pSwapChain));
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = m_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain(m_pd3dDevice, &sd, &m_pSwapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = m_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &m_pDepthStencil);
    if (FAILED(hr))
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
    if (FAILED(hr))
    {
        return hr;
    }

    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    //////////////////////////////////////////////////////////////////////////
    ID3D11BlendState* d3dBlendState;
    D3D11_BLEND_DESC omDesc;
    ZeroMemory(&omDesc, sizeof(omDesc));
    omDesc.RenderTarget[0].BlendEnable           = true;
    omDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    omDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    omDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    omDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_SRC_ALPHA;
    omDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA;
    omDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


    if (FAILED(m_pd3dDevice->CreateBlendState(&omDesc, &d3dBlendState)))
    {
        return hr;
    }

    m_pImmediateContext->OMSetBlendState(d3dBlendState, 0, 0xffffffff);

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    if (FAILED(m_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilStateDepthEnabled)))
    {
        return hr;
    }

    depthStencilDesc.DepthEnable = FALSE;
    if (FAILED(m_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilStateDepthDisabled)))
    {
        return hr;
    }

    m_pImmediateContext->OMSetDepthStencilState(m_DepthStencilStateDepthEnabled, 0);
    //////////////////////////////////////////////////////////////////////////

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pImmediateContext->RSSetViewports(1, &vp);

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    // Create the constant buffers
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBViewMatrix);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &m_pCBViewMatrix);
    if (FAILED(hr))
    {
        return hr;
    }

    bd.ByteWidth = sizeof(CBProjectionMatrix);
    hr = m_pd3dDevice->CreateBuffer(&bd, nullptr, &m_pCBProjectionMatrix);
    if (FAILED(hr))
    {
        return hr;
    }

    CBViewMatrix cbViewMatrix;
    cbViewMatrix.mView = XMMatrixTranspose(XMMatrixIdentity());
    m_pImmediateContext->UpdateSubresource(m_pCBViewMatrix, 0, nullptr, &cbViewMatrix, 0, 0);
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pCBViewMatrix);

    CBProjectionMatrix cbProjectionMatrix;
    cbProjectionMatrix.mProjection = XMMatrixTranspose(XMMatrixIdentity());
    m_pImmediateContext->UpdateSubresource(m_pCBProjectionMatrix, 0, nullptr, &cbProjectionMatrix, 0, 0);
    m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_pCBProjectionMatrix);
    
    // set clear color to my awesome purple
    m_ClearColor[0] = 0.6640625f;
    m_ClearColor[1] = 0.55859375f;
    m_ClearColor[2] = 0.8359375f;
    m_ClearColor[3] = 1.0f;

    m_ClearColor[0] = 0.0f;
    m_ClearColor[1] = 0.0f;
    m_ClearColor[2] = 0.0f;
    m_ClearColor[3] = 0.0f;

    if (!GFXPostProcessManager::GetInstance()->Init())
    {
        return E_UNEXPECTED;
    }

    if (!TwInit(TW_DIRECT3D11, m_pd3dDevice))
    {
        return E_UNEXPECTED;
    }

    TwWindowSize(width, height);

    return S_OK;
}

void GFXManager::CleanupDevice()
{
    TwTerminate();

    if (m_pImmediateContext) m_pImmediateContext->ClearState();

    GFXPostProcessManager::GetInstance()->CleanUp();

    SafeRelease(m_DepthStencilStateDepthEnabled);
    SafeRelease(m_DepthStencilStateDepthDisabled);
    SafeRelease(m_pCBViewMatrix);
    SafeRelease(m_pCBProjectionMatrix);
    SafeRelease(m_pDepthStencil);
    SafeRelease(m_pDepthStencilView);
    SafeRelease(m_pRenderTargetView);
    SafeRelease(m_pSwapChain);
    SafeRelease(m_pSwapChain1);
    SafeRelease(m_pImmediateContext1);
    SafeRelease(m_pImmediateContext);
    SafeRelease(m_pd3dDevice);
    SafeRelease(m_pd3dDevice1);
}

void GFXManager::StartRender()
{
    UpdateCameras();
    SetActiveCamera(true);
    //
    // Clear the back buffer
    //
    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, m_ClearColor);

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    GFXPostProcessManager::GetInstance()->StartRender(m_pDepthStencilView);
}

void GFXManager::StartRenderUI()
{
    SetActiveCamera(false);

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void GFXManager::EndRender()
{
    GFXPostProcessManager::GetInstance()->EndRender(m_pRenderTargetView);
    //m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    //m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    //
    // Present our back buffer to our front buffer
    //

    TwDraw();

    m_pSwapChain->Present(0, 0);
}