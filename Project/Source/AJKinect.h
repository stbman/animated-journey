//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_Kinect_H
#define AJ_Kinect_H
//---------------------------------------------------------------------------
// shared resources
#include <DirectXMath.h>

#include <Kinect.h>
#include <Kinect.Face.h>
#include <opencv2/opencv.hpp>
#include <bitset>
#include <string>

#include "AJFaceComponent.h"
#include "AJWavFormat.h"
#include "AJOBJFormat.h"

void SaveBtnCallback(int state, void* userdata);

class AJKinect
{
private:
    AJKinect();
    ~AJKinect();

    AJFaceComponent* m_FFFaceComponent;

public:
    static AJKinect* GetInstance();

    HRESULT InitKinect();
    void CleanupKinect();
    HRESULT StartFace();
    HRESULT GetFace();
    void ProcessCapturing();

    void SetIsSavedBtnPressed();

    IKinectSensor*                      m_pSensor = nullptr;
    IColorFrameSource*                  m_pColorSource = nullptr;
    IDepthFrameSource*                  m_pDepthSource = nullptr;
    IBodyFrameSource*                   m_pBodySource = nullptr;
    IAudioSource*                       m_AudioSource = nullptr;
    IAudioBeamList*                     m_AudioBeamList = nullptr;
    IAudioBeam*                         m_pAudioBeam = nullptr;
    IStream*                            m_pAudioStream = nullptr;
    IColorFrameReader*                  m_pColorReader = nullptr;
    IDepthFrameReader*                  m_pDepthReader = nullptr;
    IBodyFrameReader*                   m_pBodyReader = nullptr;
    IFrameDescription*                  m_pColorDescription = nullptr;
    IFrameDescription*                  m_pDepthDescription = nullptr;
    ICoordinateMapper*                  m_pCoordinateMapper = nullptr;
    cv::Vec3b                           m_color[BODY_COUNT];
    IHighDefinitionFaceFrameSource*     m_pHDFaceSource[BODY_COUNT];
    IHighDefinitionFaceFrameReader*     m_pHDFaceReader[BODY_COUNT];
    IFaceModelBuilder*                  m_pFaceModelBuilder[BODY_COUNT];
    bool                                m_produce[BODY_COUNT];
    bool                                m_CurrentlyTracked[BODY_COUNT];
    IFaceAlignment*                     m_pFaceAlignment[BODY_COUNT];
    IFaceModel*                         m_pFaceModel[BODY_COUNT];
    std::vector<std::vector<float>>     m_deformations;
    unsigned int                        m_colorBufferSize;
    unsigned int                        m_depthBufferSize;
    cv::Mat                             m_faceMat;
    cv::Mat                             m_colorMat;
    cv::Mat                             m_depthMat;
    cv::Mat                             m_camPosMat;
    int                                 m_pWidth = 0;
    int                                 m_pHeight   = 0;
    int                                 m_pDepthWidth = 0;
    int                                 m_pDepthHeight = 0;
    UINT32                              m_KinectFaceVertexCount     = 0;
    UINT32                              m_KinectFaceTriangleCount   = 0;
    WORD*                               m_KinectFaceTriangleIndices = nullptr;
    DirectX::XMFLOAT2*                  m_KinectFaceUVs             = nullptr;

    bool                                m_IsSavedBtnPressed         = false;
    bool                                m_KinectIsInit = false;

    // ID of timer that drives audio capture.
    static const int        cAudioReadTimerId = 1;

    // Time interval, in milliseconds, for timer that drives audio capture.
    static const int        cAudioReadTimerInterval = 50;

    // Audio samples per second in Kinect audio stream
    static const int        cAudioSamplesPerSecond = 16000;

    // Number of float samples in the audio beffer we allocate for reading every time the audio capture timer fires
    // (should be larger than the amount of audio corresponding to cAudioReadTimerInterval msec).
    static const int        cAudioBufferLength = 2 * cAudioReadTimerInterval * cAudioSamplesPerSecond / 1000;

    bool m_IsRecording = false;
    AJWavFormat m_AudioRecorder;
    AJOBJFormat m_FaceRecorder;
};

#endif