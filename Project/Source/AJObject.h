//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_OBJECT_H
#define AJ_OBJECT_H
//---------------------------------------------------------------------------

#include <vector>
#include <DirectXMath.h>

class AJBaseComponent;

class AJObject
{
private:
    std::vector<AJBaseComponent*> m_ComponentList;

    DirectX::XMVECTOR m_GlobalPosition = DirectX::XMVectorZero(); //DirectX::XMVectorZero();
    DirectX::XMVECTOR m_GlobalRotation = DirectX::XMQuaternionIdentity();
    DirectX::XMMATRIX m_World          = DirectX::XMMatrixIdentity();

    void InternalUpdate();
public:
     AJObject();
    ~AJObject();

    void AddComponent(AJBaseComponent* component);
    void RemoveComponent(AJBaseComponent* component);
    template <class searchComponent>
    searchComponent* FindComponent()
    {
        std::vector<AJBaseComponent*>::iterator end = m_ComponentList.end();
        std::vector<AJBaseComponent*>::iterator iter = m_ComponentList.begin();
        for (; iter != end; ++iter)
        {
            searchComponent* found = dynamic_cast<searchComponent*>(*iter);
            if (found)
            {
                return found;
            }
        }

        return nullptr;
    }

    void Update();

    void SetGlobalPosition(const DirectX::XMVECTOR& globalPosition);
    void SetGlobalRotation(const DirectX::XMVECTOR& globalRotation);
    DirectX::XMVECTOR GetGlobalPosition();
    DirectX::XMVECTOR GetGlobalRotation();
    DirectX::XMMATRIX GetGlobalMatrix();
};

#endif