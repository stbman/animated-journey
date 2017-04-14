//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_OBJECT_MANAGER_H
#define AJ_OBJECT_MANAGER_H
//---------------------------------------------------------------------------

#include <set>

class AJObject;
class GFXUIComponent;

class FFObjectManager
{
private:
    struct UISorter
    {
    public:
        float m_Depth = 0.0f;
        GFXUIComponent* m_UI = nullptr;

        bool operator() (const UISorter& left, const UISorter& right) const;
    };

    FFObjectManager();
    ~FFObjectManager();

    std::set<AJObject*> m_ObjectList;
public:
    static FFObjectManager* GetInstance();

    void AddObject(AJObject* obj);
    void RemoveObject(AJObject* obj);
    void ClearObjects();

    void Update();
    void Render();
    void RenderUI();
};

#endif
