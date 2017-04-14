//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJOBJFormat.h"
#include "../Source/AJObject.h"
#include "../Source/gfxheader.h"

AJOBJFormat::AJOBJFormat()
{
}

AJOBJFormat::~AJOBJFormat()
{
}

void AJOBJFormat::Init(std::wstring filePath)
{
    m_Data.clear();

    m_File.open(filePath.c_str(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);

    if (!m_File.is_open())
    {
        return;
    }

}

void AJOBJFormat::WriteBuffer(const SimpleVertex* buffer, unsigned size)
{
    //cout << "EPGNUIN PENGUIN"
    if (buffer != nullptr)
    {
        m_LastData.clear();
        for (unsigned i = 0; i < size; ++i)
        {
            m_Data.push_back(buffer[i]);
            m_LastData.push_back(buffer[i]);
        }
    }
    else
    {
        if (m_LastData.size() == size)
        {
            for (unsigned i = 0; i < size; ++i)
            {
                m_Data.push_back(m_LastData[i]);
            }
        }
        else
        {
            SimpleVertex empty;
            empty.Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
            empty.Tex = XMFLOAT2(0.0f, 0.0f);
            empty.Nor = XMFLOAT3(0.0f, 0.0f, 0.0f);

            for (unsigned i = 0; i < size; ++i)
            {
                m_Data.push_back(empty);
            }
        }
        
    }
}

void AJOBJFormat::Close()
{
    m_File.write((char*)&m_Data[0], m_Data.size() * sizeof(SimpleVertex));

    m_File.close();

    m_Data.clear();
}