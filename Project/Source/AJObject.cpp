//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJObject.h"
#include "../Source/AJMathHeader.h"

#include "../Source/AJBaseComponent.h"
//#include "../Source/gfxcomponent.h"
#include <algorithm>

#include "../Source/FFObjectManager.h"

AJObject::AJObject()
{
    FFObjectManager::GetInstance()->AddObject(this);
}

AJObject::~AJObject()
{
    for each (AJBaseComponent* component in m_ComponentList)
    {
        delete component;
    }
    m_ComponentList.clear();

    FFObjectManager::GetInstance()->RemoveObject(this);
}

void AJObject::AddComponent(AJBaseComponent* comp)
{
    if (!comp)
    {
        return;
    }

    // check if it is a GFXobj
    //GFXComponent* gfxComp = dynamic_cast<GFXComponent*>(comp);
    //if (gfxComp)
    //{
    //    // detected a gfx component
    //    // do something
    //}
    comp->m_Owner = this;
    m_ComponentList.push_back(comp);
}

void AJObject::RemoveComponent(AJBaseComponent* comp)
{
    if (!comp)
    {
        return;
    }

    //GFXComponent* gfxComp = dynamic_cast<GFXComponent*>(comp);
    //if (gfxComp)
    //{
    //    // detected a gfx component
    //    // do something
    //}

    std::vector<AJBaseComponent*>::iterator end = m_ComponentList.end();
    std::vector<AJBaseComponent*>::iterator finder = std::find(m_ComponentList.begin(), end, comp);
    if (finder == end)
    {
        return;
    }

    comp->m_Owner = nullptr;
    m_ComponentList.erase(finder);
}

void AJObject::Update()
{
    for each (AJBaseComponent* component in m_ComponentList)
    {
        if (component->GetActive())
        {
            component->Update();
        }
    }

    InternalUpdate();
}

void AJObject::InternalUpdate()
{
    m_World = XMMatrixTranslationFromVector(m_GlobalPosition) * XMMatrixRotationQuaternion(m_GlobalRotation);
}

void AJObject::SetGlobalPosition(const DirectX::XMVECTOR& globalPosition)
{
    m_GlobalPosition = globalPosition;
    InternalUpdate();
}

void AJObject::SetGlobalRotation(const DirectX::XMVECTOR& globalRotation)
{
    m_GlobalRotation = globalRotation;
    InternalUpdate();
}

DirectX::XMVECTOR AJObject::GetGlobalPosition()
{
    return m_GlobalPosition;
}

DirectX::XMVECTOR AJObject::GetGlobalRotation()
{
    return m_GlobalRotation;
}

DirectX::XMMATRIX AJObject::GetGlobalMatrix()
{
    return m_World;
}
