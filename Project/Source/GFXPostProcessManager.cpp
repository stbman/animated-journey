//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/predefine.h"
#include "../Source/GFXPostProcessManager.h"
#include "../Source/GFXPostProcessingFilter.h"
#include "../Source/AJFactoryManager.h"
#include "../Source/GFXUIComponent.h"

GFXPostProcessManager::GFXPostProcessManager() : m_IsInit(false), m_GFXMan(GFXManager::GetInstance())
{
}

GFXPostProcessManager::~GFXPostProcessManager()
{
}

GFXPostProcessManager* GFXPostProcessManager::GetInstance()
{
    static GFXPostProcessManager m_Instance;
    return &m_Instance;
}

bool GFXPostProcessManager::Init()
{
    if (m_IsInit)
    {
        return false;
    }

    m_SceneTexture = new GFXPostProcessingFilter();
    if (!m_SceneTexture)
    {
        return false;
    }
    if (   !m_SceneTexture->SetupShaders(L"../Shader/GFXShaderTexture.fx")
        || !m_SceneTexture->Init(m_GFXMan->GetWindowWidth(), m_GFXMan->GetWindowHeight()))
    {
        SafeDelete(m_SceneTexture);
        return false;
    }

    if (!InitBuffer())
    {
        SafeDelete(m_SceneTexture);
        return false;
    }

    m_IsInit = true;

    return true;
}

void GFXPostProcessManager::StartRender(ID3D11DepthStencilView* depthStencilView)
{
    if (!m_IsInit)
    {
        return;
    }

    m_CachedDepthStencilView = depthStencilView;
    m_SceneTexture->SetRenderTarget(depthStencilView);
    
    m_SceneTexture->ClearRenderTarget(depthStencilView, m_GFXMan->GetClearBlue(), m_GFXMan->GetClearGreen(), m_GFXMan->GetClearRed(), m_GFXMan->GetClearAlpha());
}

void GFXPostProcessManager::EndRender(ID3D11RenderTargetView* renderBuffer)
{
    if (!m_IsInit)
    {
        return;
    }

    m_GFXMan->SetActiveCamera(false);
    ID3D11DeviceContext* immediateContext = m_GFXMan->GetD3DDeviceContext();

    ID3D11ShaderResourceView* currentShaderResourceView = m_SceneTexture->GetShaderResourceView();

    // TODO: make it work for my filters
    for each(GFXPostProcessingFilter* filter in m_Filters)
    {
        immediateContext->ClearDepthStencilView(m_CachedDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        filter->Update();
        
        filter->SetRenderTarget(m_CachedDepthStencilView);
        
        filter->ClearRenderTarget(m_CachedDepthStencilView, m_GFXMan->GetClearBlue(), m_GFXMan->GetClearGreen(), m_GFXMan->GetClearRed(), m_GFXMan->GetClearAlpha());

        // setup shader for that filter

        m_QuadComponent->SwapTextureResource(currentShaderResourceView);
        m_QuadComponent->SetupShaders(filter->GetVertexShader(), filter->GetPixelShader(), filter->GetInputLayout());
        m_QuadComponent->Render();
        m_QuadComponent->SetupShaders(nullptr, nullptr, nullptr);
        m_QuadComponent->SwapTextureResource(nullptr);

        currentShaderResourceView = filter->GetShaderResourceView();
    }

    immediateContext->ClearDepthStencilView(m_CachedDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    immediateContext->OMSetRenderTargets(1, &renderBuffer, m_CachedDepthStencilView);

    m_QuadComponent->SwapTextureResource(currentShaderResourceView);
    m_QuadComponent->SetupShaders(m_SceneTexture->GetVertexShader(), m_SceneTexture->GetPixelShader(), m_SceneTexture->GetInputLayout());
    m_QuadComponent->Render();
    m_QuadComponent->SetupShaders(nullptr, nullptr, nullptr);
    m_QuadComponent->SwapTextureResource(nullptr);

    // reset camera
    m_GFXMan->SetActiveCamera(true);
}

void GFXPostProcessManager::CleanUp()
{
    m_IsInit = false;

    std::list<GFXPostProcessingFilter*> toDelete = m_Filters;
    for each(GFXPostProcessingFilter* filter in toDelete)
    {
        RemovePostProcessingFilter(filter);
    }

    SafeDelete(m_SceneTexture);

    SafeDelete(m_QuadComponent);
}

bool GFXPostProcessManager::InitBuffer()
{
    int screenWidth  = m_GFXMan->GetWindowWidth();
    int screenHeight = m_GFXMan->GetWindowHeight();
    
    m_QuadComponent = AJFactory::CreateQuadComponent(0, (float)screenWidth, 0, (float)screenHeight);

    return true;
}

void GFXPostProcessManager::AddPostProcessingFilter(GFXPostProcessingFilter* filter)
{
    if (filter == nullptr)
    {
        return;
    }

    m_Filters.push_back(filter);

    if (m_IsInit)
    {
        // init filter
        filter->Init(m_GFXMan->GetWindowWidth(), m_GFXMan->GetWindowHeight());
    }
}

void GFXPostProcessManager::RemovePostProcessingFilter(GFXPostProcessingFilter* filter)
{
    if (filter == nullptr)
    {
        return;
    }

    m_Filters.remove(filter);
    
    if (m_IsInit)
    {
        // clean up filter
        filter->Shutdown();
    }

}
