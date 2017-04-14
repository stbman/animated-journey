//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/FFObjectManager.h"
#include "../Source/AJObject.h"
#include "../Source/gfxcomponent.h"
#include "../Source/GFXUIComponent.h"
//#include "../Source/gfxmanager.h"
#include <vector>
#include <algorithm>

FFObjectManager::FFObjectManager()
{
}

FFObjectManager::~FFObjectManager()
{
}

FFObjectManager* FFObjectManager::GetInstance()
{
    static FFObjectManager m_Instance;
    return &m_Instance;
}

void FFObjectManager::AddObject(AJObject* obj)
{
    m_ObjectList.insert(obj);
}

void FFObjectManager::RemoveObject(AJObject* obj)
{
    m_ObjectList.erase(obj);
}

void FFObjectManager::ClearObjects()
{
    std::set<AJObject*>::iterator iter = m_ObjectList.begin();
    while (iter != m_ObjectList.end())
    {
        AJObject* obj = *(iter);
        ++iter;

        delete obj;
    }
}

void FFObjectManager::Update()
{
    for each (AJObject* obj in m_ObjectList)
    {
        obj->Update();
    }
}

void FFObjectManager::Render()
{
    for each (AJObject* obj in m_ObjectList)
    {
        GFXComponent* gfxComp = obj->FindComponent<GFXComponent>();
        if (   gfxComp
            && gfxComp->GetActive())
        {
            gfxComp->GFXUpdate();
            gfxComp->Render();
        }
    }
}

bool FFObjectManager::UISorter::operator() (const UISorter& left, const UISorter& right) const
{
    return left.m_Depth < right.m_Depth;
}

void FFObjectManager::RenderUI()
{
    GFXManager::GetInstance()->SetDepth(false);

    std::vector<UISorter> uiElements;
    for each (AJObject* obj in m_ObjectList)
    {
        GFXUIComponent* gfxUIComp = obj->FindComponent<GFXUIComponent>();
        if (   gfxUIComp
            && gfxUIComp->GetActive())
        {
            UISorter data;
            data.m_Depth = XMVectorGetZ(obj->GetGlobalPosition());
            data.m_UI    = gfxUIComp;
            uiElements.push_back(data);
        }
    }

    std::sort(uiElements.begin(), uiElements.end(), UISorter());

    for each (auto element in uiElements)
    {
        element.m_UI->GFXUpdate();
        element.m_UI->Render();
    }

    GFXManager::GetInstance()->SetDepth(true);
}