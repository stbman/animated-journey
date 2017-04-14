//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJFaceComponent.h"
#include "../Source/AJKinect.h"
#include "../Source/gfxbasecomponent.h"

#include "../Source/gfxcamera.h"
#include "../Source/AJKeyboardManager.h"

#include <iostream>
#include <fstream>
#include <string>

AJFaceComponent::AJFaceComponent() : m_KinectCache(AJKinect::GetInstance()), m_Index(-1)
{
}

void AJFaceComponent::Update()
{
    GFXBaseComponent* gfxComp = m_Owner->FindComponent<GFXBaseComponent>();
    if (!gfxComp)
    {
        return;
    }

    AJKeyboardManager* keyMan = AJKeyboardManager::GetInstance();
    if (keyMan->Triggered(AJKeyboardManager::KEY_0))
    {
        //gfxComp->SetupTexture("FACE IMAGE 0");
    }
    else if (keyMan->Triggered(AJKeyboardManager::KEY_1))
    {
        //gfxComp->SetupTexture("FACE IMAGE 1");
    }
    else if (keyMan->Triggered(AJKeyboardManager::KEY_2))
    {
        //gfxComp->SetupTexture("FACE IMAGE 2");
    }

    if (m_Index < 0)
    {
        return;
    }

    // If not tracked, don't update and render
    if (!m_KinectCache->m_CurrentlyTracked[m_Index])
    {
        gfxComp->SetActive(false);
        return;
    }

    gfxComp->SetActive(true);

    UINT32 kinectFaceVertexCount = m_KinectCache->m_KinectFaceVertexCount;
    std::vector<CameraSpacePoint> facePoints(kinectFaceVertexCount);
    // Return face vertices in kinect space aligned to face
    HRESULT hResult = m_KinectCache->m_pFaceModel[m_Index]->CalculateVerticesForAlignment(m_KinectCache->m_pFaceAlignment[m_Index], kinectFaceVertexCount, &facePoints[0]);
    if (SUCCEEDED(hResult))
    {
        // grab UVs from the file uv
        DirectX::XMFLOAT2* kinectFaceUVs = m_KinectCache->m_KinectFaceUVs;
        // setup positional information
        m_vertexData = new SimpleVertex[kinectFaceVertexCount];
        for (UINT32 i = 0; i < kinectFaceVertexCount; ++i)
        {
            CameraSpacePoint data = facePoints[i];
            m_vertexData[i].Nor = XMFLOAT3(0.0f, 0.0f, 0.0f);
            m_vertexData[i].Pos = XMFLOAT3(-data.X, data.Y, data.Z);
            
            m_vertexData[i].Tex = kinectFaceUVs[i];
        }

        // setup indices information
        UINT32 kinectFaceTriangleCount = m_KinectCache->m_KinectFaceTriangleCount;
        UINT32 indicesCount = kinectFaceTriangleCount * 3;
        WORD* indicesData = m_KinectCache->m_KinectFaceTriangleIndices;
        m_newIndicesData = new WORD[indicesCount];
        memcpy(m_newIndicesData, indicesData, sizeof(WORD) * indicesCount);

        // setup normal information
        UINT32* normalCount = new UINT32[kinectFaceVertexCount];
        memset(normalCount, 0, sizeof(UINT32) * kinectFaceVertexCount);
        for (UINT32 i = 0; i < kinectFaceTriangleCount ; ++i)
        {
            UINT32 baseIndex = i * 3;
            UINT32 indexC   = m_newIndicesData[baseIndex];
            UINT32 indexB   = m_newIndicesData[baseIndex + 1];
            UINT32 indexA   = m_newIndicesData[baseIndex + 2];

            m_newIndicesData[baseIndex]     = (WORD)indexA;
            m_newIndicesData[baseIndex + 2] = (WORD)indexC;

            ++normalCount[indexA];
            ++normalCount[indexB];
            ++normalCount[indexC];

            XMVECTOR vectorA = XMLoadFloat3(&m_vertexData[indexA].Pos);
            XMVECTOR vectorB = XMLoadFloat3(&m_vertexData[indexB].Pos);
            XMVECTOR vectorC = XMLoadFloat3(&m_vertexData[indexC].Pos);

            XMVECTOR vecA = XMVectorSubtract(vectorA, vectorB);
            XMVECTOR vecC = XMVectorSubtract(vectorC, vectorB);
            XMVECTOR normal = XMVector3Normalize(XMVector3Cross(vecA, vecC));

            XMVECTOR newA = XMLoadFloat3(&m_vertexData[indexA].Nor);
            XMVECTOR newB = XMLoadFloat3(&m_vertexData[indexB].Nor);
            XMVECTOR newC = XMLoadFloat3(&m_vertexData[indexC].Nor);

            newA = XMVectorAdd(newA, normal);
            newB = XMVectorAdd(newB, normal);
            newC = XMVectorAdd(newC, normal);

            XMStoreFloat3(&m_vertexData[indexA].Nor, newA);
            XMStoreFloat3(&m_vertexData[indexB].Nor, newB);
            XMStoreFloat3(&m_vertexData[indexC].Nor, newC);
        }
        for (UINT32 i = 0; i < kinectFaceVertexCount; ++i)
        {
            XMVECTOR normal = XMLoadFloat3(&m_vertexData[i].Nor);
            // Normalise with number of times we add 
            normal = XMVector3Normalize(XMVectorScale(normal, 1.0f / static_cast<float>(normalCount[i])));
            XMStoreFloat3(&m_vertexData[i].Nor, normal);
        }

        gfxComp->UpdateVertices(m_vertexData, kinectFaceVertexCount);
        gfxComp->UpdateIndices(m_newIndicesData, indicesCount);

        //// write to file
        //static int fileCount = 0;
        //std::string fileName = "faceModel_" + std::to_string(fileCount) + ".obj";
        //std::ofstream myfile;
        //myfile.open(fileName.c_str());
        //for (UINT32 i = 0; i < kinectFaceVertexCount; ++i)
        //{
        //    myfile << "v " << vertexData[i].Pos.x << " " << vertexData[i].Pos.y << " " << vertexData[i].Pos.z << "1.0" << std::endl;
        //}
        //for (UINT32 i = 0; i < kinectFaceTriangleCount; ++i)
        //{
        //    UINT32 baseIndex = i * 3;
        //    UINT32 indexA = newIndicesData[baseIndex] + 1;
        //    UINT32 indexB = newIndicesData[baseIndex + 1] + 1;
        //    UINT32 indexC = newIndicesData[baseIndex + 2] + 1;
        //    myfile << "f " << indexA << " " << indexB << " " << indexC << std::endl;
        //}
        //myfile.close();
        //++fileCount;

        //GFXCamera* camera = GFXManager::GetInstance()->GetActiveCamera();
        //CameraSpacePoint data = facePoints[0];
        //camera->SetCameraPosition(XMVectorSet(data.X, data.Y, data.Z -1.0f, 0.0f));
        //camera->SetCameraLookAt(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
        delete normalCount;
        //delete newIndicesData;
        //delete vertexData;
    }
}

void AJFaceComponent::Init(int index)
{
    m_Index = index;
}

void AJFaceComponent::SaveIndices()
{
    // write to file
    //static int fileCount = 0;
    //std::string fileName = "faceModel_" + std::to_string(fileCount) + ".obj";
    //std::ofstream myfile;
    //myfile.open(fileName.c_str());
    //for (UINT32 i = 0; i < m_KinectCache->m_KinectFaceVertexCount; ++i)
    //{
    //    myfile << "v " << m_vertexData[i].Pos.x << " " << m_vertexData[i].Pos.y << " " << m_vertexData[i].Pos.z << "1.0" << std::endl;
    //}
    //
    //for (UINT32 i = 0; i < m_KinectCache->m_KinectFaceTriangleCount; ++i)
    //{
    //    UINT32 baseIndex = i * 3;
    //    UINT32 indexA = m_newIndicesData[baseIndex] + 1;
    //    UINT32 indexB = m_newIndicesData[baseIndex + 1] + 1;
    //    UINT32 indexC = m_newIndicesData[baseIndex + 2] + 1;
    //    myfile << "f " << indexA << " " << indexB << " " << indexC << std::endl;
    //}
    //myfile.close();
    //++fileCount;
}

bool AJFaceComponent::IsTracked()
{
    return    m_Index >= 0
           && m_Index <  6
           && m_KinectCache != nullptr
           && m_KinectCache->m_CurrentlyTracked[m_Index];
}