//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef AJ_GFX_POST_PROCESS_MANAGER_H
#define AJ_GFX_POST_PROCESS_MANAGER_H
//---------------------------------------------------------------------------

#include <list>

class GFXManager;
class GFXPostProcessingFilter;
struct ID3D11DepthStencilView;
class GFXUIComponent;

class GFXPostProcessManager
{
private:
     GFXPostProcessManager();
    ~GFXPostProcessManager();

    GFXPostProcessingFilter* m_SceneTexture = nullptr;
    std::list<GFXPostProcessingFilter*> m_Filters;
    bool m_IsInit;

    GFXUIComponent* m_QuadComponent = nullptr;
    // cache
    GFXManager* m_GFXMan = nullptr;
    ID3D11DepthStencilView* m_CachedDepthStencilView = nullptr;

    bool InitBuffer();
public:
    static GFXPostProcessManager* GetInstance();

    template <class searchFilter>
    searchFilter* FindFilter()
    {
        std::list<GFXPostProcessingFilter*>::iterator end  = m_Filters.end();
        std::list<GFXPostProcessingFilter*>::iterator iter = m_Filters.begin();
        for (; iter != end; ++iter)
        {
            searchFilter* found = dynamic_cast<searchFilter*>(*iter);
            if (found)
            {
                return found;
            }
        }

        return nullptr;
    }

    bool Init();
    void StartRender(ID3D11DepthStencilView* depthStencilView);
    void EndRender(ID3D11RenderTargetView* renderBuffer);
    void CleanUp();

    void AddPostProcessingFilter(GFXPostProcessingFilter* filter);
    void RemovePostProcessingFilter(GFXPostProcessingFilter* filter);
};

#endif