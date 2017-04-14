//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/predefine.h"
#include "../Source/AJKinect.h"
#include <iostream>
#include <fstream>

AJKinect::AJKinect()
{
    memset(m_CurrentlyTracked, false, sizeof(m_CurrentlyTracked));
    memset(m_produce,          false, sizeof(m_produce));
    memset(m_pFaceAlignment,   false, sizeof(m_pFaceAlignment));
    memset(m_pFaceModel,       false, sizeof(m_pFaceModel));
    m_deformations = std::vector<std::vector<float>>(BODY_COUNT, std::vector<float>(FaceShapeDeformations::FaceShapeDeformations_Count));

}

AJKinect::~AJKinect()
{
}

AJKinect* AJKinect::GetInstance()
{
    static AJKinect m_Instance;
    return &m_Instance;
}

void SaveBtnCallback(int state, void* userdata)
{
    state;
    userdata;
    AJKinect::GetInstance()->SetIsSavedBtnPressed();
}

void AJKinect::SetIsSavedBtnPressed()
{
    m_IsSavedBtnPressed = !m_IsSavedBtnPressed;
}

HRESULT AJKinect::InitKinect()
{
    cv::setUseOptimized(true);

    // Sensor
    HRESULT hResult = S_OK;
    hResult = GetDefaultKinectSensor(&m_pSensor);
    if (FAILED(hResult))
    {
        std::cerr << "Error : GetDefaultKinectSensor" << std::endl;
        return -1;
    }

    hResult = m_pSensor->Open();
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::Open()" << std::endl;
        return -1;
    }

    // audio
    hResult = m_pSensor->get_AudioSource(&m_AudioSource);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::get_AudioSource()" << std::endl;
        return -1;
    }

    hResult = m_AudioSource->get_AudioBeams(&m_AudioBeamList);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::get_AudioBeams()" << std::endl;
        return -1;
    }

    hResult = m_AudioBeamList->OpenAudioBeam(0, &m_pAudioBeam);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::OpenAudioBeam()" << std::endl;
        return -1;
    }

    hResult = m_pAudioBeam->OpenInputStream(&m_pAudioStream);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::OpenInputStream()" << std::endl;
        return -1;
    }

    // Source
    hResult = m_pSensor->get_ColorFrameSource(&m_pColorSource);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::get_ColorFrameSource()" << std::endl;
        return -1;
    }

    hResult = m_pSensor->get_DepthFrameSource(&m_pDepthSource);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::get_DepthFrameSource()" << std::endl;
        return -1;
    }

    hResult = m_pSensor->get_BodyFrameSource(&m_pBodySource);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::get_BodyFrameSource()" << std::endl;
        return -1;
    }

    // Reader
    hResult = m_pColorSource->OpenReader(&m_pColorReader);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IColorFrameSource::OpenReader()" << std::endl;
        return -1;
    }

    hResult = m_pDepthSource->OpenReader(&m_pDepthReader);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IColorFrameSource::OpenReader()" << std::endl;
        return -1;
    }

    hResult = m_pBodySource->OpenReader(&m_pBodyReader);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
        return -1;
    }

    // Description
    hResult = m_pColorSource->get_FrameDescription(&m_pColorDescription);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IColorFrameSource::get_FrameDescription()" << std::endl;
        return -1;
    }

    hResult = m_pDepthSource->get_FrameDescription(&m_pDepthDescription);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IDepthFrameSource::get_FrameDescription()" << std::endl;
        return -1;
    }

    m_pColorDescription->get_Width(&m_pWidth); // 1920
    m_pColorDescription->get_Height(&m_pHeight); // 1080
    m_colorBufferSize = m_pWidth * m_pHeight * 4 * sizeof(unsigned char);

    m_colorMat = cv::Mat(m_pHeight, m_pWidth, CV_8UC4);
    m_faceMat  = cv::Mat(m_pHeight / 2, m_pWidth / 2, CV_8UC4);

    m_pDepthDescription->get_Width(&m_pDepthWidth); // ?
    m_pDepthDescription->get_Height(&m_pDepthHeight); // ?
    m_depthBufferSize = m_pDepthWidth * m_pDepthHeight;// *sizeof(UINT16);

    unsigned int byteperpixel;
    unsigned int lengthpixel;
    m_pDepthDescription->get_BytesPerPixel(&byteperpixel);
    m_pDepthDescription->get_LengthInPixels(&lengthpixel);

    m_depthMat = cv::Mat(m_pDepthHeight, m_pDepthWidth, CV_16U);
    m_camPosMat = cv::Mat(m_pHeight, m_pWidth, CV_32FC3);

    // Color Table
    m_color[0] = cv::Vec3b(255,   0,   0);
    m_color[1] = cv::Vec3b(0  , 255,   0);
    m_color[2] = cv::Vec3b(0  ,   0, 255);
    m_color[3] = cv::Vec3b(255, 255,   0);
    m_color[4] = cv::Vec3b(255,   0, 255);
    m_color[5] = cv::Vec3b(0  , 255, 255);
	 
    // Coordinate Mapper
    hResult = m_pSensor->get_CoordinateMapper(&m_pCoordinateMapper);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::get_CoordinateMapper()" << std::endl;
        return -1;
    }

    cv::namedWindow("Kinect Monitor");
    cv::createButton("Save State", SaveBtnCallback, NULL, 0, false);

    for (int count = 0; count < BODY_COUNT; count++)
    {
        // Source
        hResult = CreateHighDefinitionFaceFrameSource(m_pSensor, &m_pHDFaceSource[count]);
        if (FAILED(hResult))
        {
            std::cerr << "Error : CreateHighDefinitionFaceFrameSource()" << std::endl;
            return -1;
        }

        // Reader
        hResult = m_pHDFaceSource[count]->OpenReader(&m_pHDFaceReader[count]);
        if (FAILED(hResult))
        {
            std::cerr << "Error : IHighDefinitionFaceFrameSource::OpenReader()" << std::endl;
            return -1;
        }

        // Open Face Model Builder
        hResult = m_pHDFaceSource[count]->OpenModelBuilder(FaceModelBuilderAttributes::FaceModelBuilderAttributes_None, &m_pFaceModelBuilder[count]);
        if (FAILED(hResult))
        {
            std::cerr << "Error : IHighDefinitionFaceFrameSource::OpenModelBuilder()" << std::endl;
            return -1;
        }

        // Start Collection Face Data
        hResult = m_pFaceModelBuilder[count]->BeginFaceDataCollection();
        if (FAILED(hResult))
        {
            std::cerr << "Error : IFaceModelBuilder::BeginFaceDataCollection()" << std::endl;
            return -1;
        }

        // Create Face Alignment
        hResult = CreateFaceAlignment(&m_pFaceAlignment[count]);
        if (FAILED(hResult))
        {
            std::cerr << "Error : CreateFaceAlignment()" << std::endl;
            return -1;
        }

        // Create Face Model
        hResult = CreateFaceModel(1.0f, FaceShapeDeformations::FaceShapeDeformations_Count, &m_deformations[count][0], &m_pFaceModel[count]);
        if (FAILED(hResult))
        {
            std::cerr << "Error : CreateFaceModel()" << std::endl;
            return -1;
        }
    }

    hResult = GetFaceModelVertexCount(&m_KinectFaceVertexCount); // 1347
    if (FAILED(hResult))
    {
        std::cerr << "Error : GetFaceModelVertexCount()" << std::endl;
        return -1;
    }

    hResult = GetFaceModelTriangleCount(&m_KinectFaceTriangleCount); // 2630
    if (FAILED(hResult))
    {
        std::cerr << "Error : GetFaceModelTriangleCount()" << std::endl;
        return -1;
    }

    m_KinectFaceTriangleIndices = new WORD[m_KinectFaceTriangleCount * 3];
    if (!m_KinectFaceTriangleIndices)
    {
        std::cerr << "Error : Unable to allocate memory for triangle indices" << std::endl;
        return -1;
    }
    std::memset(m_KinectFaceTriangleIndices, 0, sizeof(WORD));

    UINT32* faceTriangleIndices = new UINT32[m_KinectFaceTriangleCount * 3];
    if (!faceTriangleIndices)
    {
        std::cerr << "Error : Unable to allocate memory for triangle indices" << std::endl;
        return -1;
    }
    std::memset(faceTriangleIndices, 0, sizeof(UINT32));

    hResult = GetFaceModelTriangles(m_KinectFaceTriangleCount * 3, faceTriangleIndices);
    if (FAILED(hResult))
    {
        delete faceTriangleIndices;
        std::cerr << "Error : GetFaceModelTriangles()" << std::endl;
        return -1;
    }

    for (UINT32 iter = 0; iter < (m_KinectFaceTriangleCount * 3); ++iter)
    {
        m_KinectFaceTriangleIndices[iter] = (WORD)faceTriangleIndices[iter];
    }
    delete faceTriangleIndices;

    m_KinectFaceUVs = new XMFLOAT2[m_KinectFaceVertexCount];
    if (!m_KinectFaceUVs)
    {
        std::cerr << "Error : Unable to allocate memory for triangle UVs" << std::endl;
        return -1;
    }
    else
    {
        std::ifstream infile;
        infile.open("../Resource/HDFaceData_UV.txt");
        if (infile.is_open())
        {
            for (UINT32 i = 0; i < m_KinectFaceVertexCount; ++i)
            {
                float u = 0.0f;
                float v = 0.0f;
                infile >> u >> v;
                m_KinectFaceUVs[i].x = u;
                m_KinectFaceUVs[i].y = v;
            }
            infile.close();
        }
    }

    SetTimer(GFXManager::GetInstance()->GetHWND(), cAudioReadTimerId, cAudioReadTimerInterval, NULL);

    return S_OK;
}

void AJKinect::CleanupKinect()
{
    KillTimer(GFXManager::GetInstance()->GetHWND(), AJKinect::cAudioReadTimerId);

    SafeDelete(m_KinectFaceTriangleIndices);
    SafeDelete(m_KinectFaceUVs);
    SafeRelease(m_AudioSource);
    SafeRelease(m_AudioBeamList);
    SafeRelease(m_pAudioBeam);
    SafeRelease(m_pAudioStream);
    SafeRelease(m_pColorSource);
    SafeRelease(m_pDepthSource);
    SafeRelease(m_pBodySource);
    SafeRelease(m_pColorReader);
    SafeRelease(m_pDepthReader);
    SafeRelease(m_pBodyReader);
    SafeRelease(m_pColorDescription);
    SafeRelease(m_pDepthDescription);
    SafeRelease(m_pCoordinateMapper);
    for (int count = 0; count < BODY_COUNT; count++)
    {
        SafeRelease(m_pHDFaceSource[count]);
        SafeRelease(m_pHDFaceReader[count]);
        SafeRelease(m_pFaceModelBuilder[count]);
        SafeRelease(m_pFaceAlignment[count]);
        SafeRelease(m_pFaceModel[count]);
    }
    if (m_pSensor)
    {
        m_pSensor->Close();
    }
    SafeRelease(m_pSensor);
    cv::destroyAllWindows();
}

HRESULT AJKinect::StartFace()
{
    HRESULT hResult = S_OK;

    bool colorReady = false;
    bool depthReady = false;

    // Color Frame
    IColorFrame* pColorFrame = nullptr;
    hResult = m_pColorReader->AcquireLatestFrame(&pColorFrame);
    if (SUCCEEDED(hResult))
    {
        hResult = pColorFrame->CopyConvertedFrameDataToArray(m_colorBufferSize, reinterpret_cast<BYTE*>(m_colorMat.data), ColorImageFormat::ColorImageFormat_Bgra);
        if (SUCCEEDED(hResult))
        {
            colorReady = true;
            cv::resize(m_colorMat, m_faceMat, cv::Size(), 0.5, 0.5);
        }
    }
    SafeRelease(pColorFrame);

    // Depth Frame
    IDepthFrame* pDepthFrame = nullptr;
    hResult = m_pDepthReader->AcquireLatestFrame(&pDepthFrame);
    if (SUCCEEDED(hResult))
    {
        hResult = pDepthFrame->CopyFrameDataToArray(m_depthBufferSize, reinterpret_cast<UINT16*>(m_depthMat.data));
        if (SUCCEEDED(hResult))
        {
            hResult = m_pCoordinateMapper->MapColorFrameToCameraSpace(m_depthBufferSize, (UINT16*)m_depthMat.data, m_colorBufferSize / 4, (CameraSpacePoint*)m_camPosMat.data);
            if (SUCCEEDED(hResult))
            {
                depthReady = true;
            }
        }
    }
    SafeRelease(pDepthFrame);

    if (colorReady && depthReady)
    {
        m_KinectIsInit = true;
        //
        //UINT colorPixelCount = m_pWidth      * m_pHeight;
        //UINT depthPixelCount = m_pDepthWidth * m_pDepthHeight;
        //
        //DepthSpacePoint* depthData = new DepthSpacePoint[colorPixelCount];
        //
        //hResult = m_pCoordinateMapper->MapColorFrameToDepthSpace(depthPixelCount, (UINT16*)m_depthMat.data, colorPixelCount, depthData);
        //if (SUCCEEDED(hResult))
        //{
        //    cv::Mat mappedDepth = cv::Mat(m_pHeight, m_pWidth, CV_16UC1);
        //    for (UINT colorY = 0; colorY < kinect->m_pHeight; ++colorY)
        //    {
        //        for (UINT colorX = 0; colorX < kinect->m_pWidth; ++colorX)
        //        {
        //        }
        //    }
        //}
        //
        //delete depthData;
    }
    return hResult;
}

HRESULT AJKinect::GetFace()
{
    HRESULT hResult = S_OK;

    // Body Frame
    IBodyFrame* pBodyFrame = nullptr;
    hResult = m_pBodyReader->AcquireLatestFrame(&pBodyFrame);
    if (SUCCEEDED(hResult))
    {
        IBody* pBody[BODY_COUNT] = { 0 };
        hResult = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, pBody);
        if (SUCCEEDED(hResult))
        {
            for (int count = 0; count < BODY_COUNT; count++)
            {
                BOOLEAN bTrackingIdValid = false;
                hResult = m_pHDFaceSource[count]->get_IsTrackingIdValid(&bTrackingIdValid);
                if (!bTrackingIdValid)
                {
                    BOOLEAN bTracked = false;
                    hResult = pBody[count]->get_IsTracked(&bTracked);
                    if (   SUCCEEDED(hResult)
                        && bTracked)
                    {
                        // Set TrackingID to Detect Face
                        UINT64 trackingId = _UI64_MAX;
                        hResult = pBody[count]->get_TrackingId(&trackingId);
                        if (SUCCEEDED(hResult))
                        {
                            m_pHDFaceSource[count]->put_TrackingId(trackingId);
                        }
                    }
                }
            }
        }
        for (int count = 0; count < BODY_COUNT; count++)
        {
            SafeRelease(pBody[count]);
        }
    }
    SafeRelease(pBodyFrame);

    // HD Face Frame
    for (int count = 0; count < BODY_COUNT; count++)
    {
        m_CurrentlyTracked[count] = false;
        IHighDefinitionFaceFrame* pHDFaceFrame = nullptr;
        hResult = m_pHDFaceReader[count]->AcquireLatestFrame(&pHDFaceFrame);
        if (   SUCCEEDED(hResult)
            && pHDFaceFrame != nullptr)
        {
            BOOLEAN bFaceTracked = false;
            hResult = pHDFaceFrame->get_IsFaceTracked(&bFaceTracked);
            if (   SUCCEEDED(hResult)
                && bFaceTracked)
            {
                hResult = pHDFaceFrame->GetAndRefreshFaceAlignmentResult(m_pFaceAlignment[count]);
                if (   SUCCEEDED(hResult)
                    && m_pFaceAlignment[count] != nullptr)
                {
                    m_CurrentlyTracked[count] = true;
                    // Face Model Building
                    if (!m_produce[count])
                    {
                        //std::system("cls");
                        FaceModelBuilderCollectionStatus collection;
                        hResult = m_pFaceModelBuilder[count]->get_CollectionStatus(&collection);
                        if (collection == FaceModelBuilderCollectionStatus::FaceModelBuilderCollectionStatus_Complete)
                        {
                            //std::cout << "Status : Complete" << std::endl;
                            //cv::putText(m_colorMat, "Status : Complete", cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0f, static_cast<cv::Scalar>(m_color[count]), 2, CV_AA);
                            IFaceModelData* pFaceModelData = nullptr;
                            hResult = m_pFaceModelBuilder[count]->GetFaceData(&pFaceModelData);
                            if (   SUCCEEDED(hResult)
                                && pFaceModelData != nullptr)
                            {
                                hResult = pFaceModelData->ProduceFaceModel(&m_pFaceModel[count]);
                                if (   SUCCEEDED(hResult)
                                    && m_pFaceModel[count] != nullptr)
                                {
                                    m_produce[count] = true;
                                }
                            }
                            SafeRelease(pFaceModelData);
                        }
                        //else
                        //{
                        //    //std::cout << "Status : " << collection << std::endl;
                        //    cv::putText(m_colorMat, "Status : " + std::to_string(collection), cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0f, static_cast<cv::Scalar>(m_color[count]), 2, CV_AA);
                        //
                        //    // Collection Status
                        //    if (collection >= FaceModelBuilderCollectionStatus::FaceModelBuilderCollectionStatus_TiltedUpViewsNeeded)
                        //    {
                        //        //std::cout << "Need : Tilted Up Views" << std::endl;
                        //        cv::putText(m_colorMat, "Need : Tilted Up Views", cv::Point(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0f, static_cast<cv::Scalar>(m_color[count]), 2, CV_AA);
                        //    }
                        //    else if (collection >= FaceModelBuilderCollectionStatus::FaceModelBuilderCollectionStatus_RightViewsNeeded)
                        //    {
                        //        //std::cout << "Need : Right Views" << std::endl;
                        //        cv::putText(m_colorMat, "Need : Right Views", cv::Point(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0f, static_cast<cv::Scalar>(m_color[count]), 2, CV_AA);
                        //    }
                        //    else if (collection >= FaceModelBuilderCollectionStatus::FaceModelBuilderCollectionStatus_LeftViewsNeeded)
                        //    {
                        //        //std::cout << "Need : Left Views" << std::endl;
                        //        cv::putText(m_colorMat, "Need : Left Views", cv::Point(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0f, static_cast<cv::Scalar>(m_color[count]), 2, CV_AA);
                        //    }
                        //    else if (collection >= FaceModelBuilderCollectionStatus::FaceModelBuilderCollectionStatus_FrontViewFramesNeeded)
                        //    {
                        //        //std::cout << "Need : Front ViewFrames" << std::endl;
                        //        cv::putText(m_colorMat, "Need : Front ViewFrames", cv::Point(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0f, static_cast<cv::Scalar>(m_color[count]), 2, CV_AA);
                        //    }
                        //
                        //    // Capture Status
                        //    FaceModelBuilderCaptureStatus capture;
                        //    hResult = m_pFaceModelBuilder[count]->get_CaptureStatus(&capture);
                        //    switch (capture)
                        //    {
                        //    case FaceModelBuilderCaptureStatus::FaceModelBuilderCaptureStatus_FaceTooFar:
                        //        //std::cout << "Error : Face Too Far from Camera" << std::endl;
                        //        cv::putText(m_colorMat, "Error : Face Too Far from Camera", cv::Point(50, 150), cv::FONT_HERSHEY_SIMPLEX, 1.0f, static_cast<cv::Scalar>(m_color[count]), 2, CV_AA);
                        //        break;
                        //    case FaceModelBuilderCaptureStatus::FaceModelBuilderCaptureStatus_FaceTooNear:
                        //        //std::cout << "Error : Face Too Near to Camera" << std::endl;
                        //        cv::putText(m_colorMat, "Error : Face Too Near to Camera", cv::Point(50, 150), cv::FONT_HERSHEY_SIMPLEX, 1.0f, static_cast<cv::Scalar>(m_color[count]), 2, CV_AA);
                        //        break;
                        //    case FaceModelBuilderCaptureStatus_MovingTooFast:
                        //        //std::cout << "Error : Moving Too Fast" << std::endl;
                        //        cv::putText(m_colorMat, "Error : Moving Too Fast", cv::Point(50, 150), cv::FONT_HERSHEY_SIMPLEX, 1.0f, static_cast<cv::Scalar>(m_color[count]), 2, CV_AA);
                        //        break;
                        //    default:
                        //        break;
                        //    }
                        //}
                    }

                    // HD Face Points
                    //std::vector<CameraSpacePoint> facePoints(m_KinectFaceVertexCount);
                    //hResult = m_pFaceModel[count]->CalculateVerticesForAlignment(m_pFaceAlignment[count], m_KinectFaceVertexCount, &facePoints[0]);
                    //if (SUCCEEDED(hResult))
                    //{
                    //    for (UINT32 point = 0; point < m_KinectFaceVertexCount; point++)
                    //    {
                    //        ColorSpacePoint colorSpacePoint;
                    //        hResult = m_pCoordinateMapper->MapCameraPointToColorSpace(facePoints[point], &colorSpacePoint);
                    //        if (SUCCEEDED(hResult))
                    //        {
                    //            int x = static_cast<int>(colorSpacePoint.X);
                    //            int y = static_cast<int>(colorSpacePoint.Y);
                    //            if (   (x >= 0       )
                    //                && (x < m_pWidth )
                    //                && (y >= 0       )
                    //                && (y < m_pHeight))
                    //            {
                    //                cv::circle(m_colorMat, cv::Point(static_cast<int>(colorSpacePoint.X), static_cast<int>(colorSpacePoint.Y)), 5, static_cast<cv::Scalar>(m_color[count]), -1, CV_AA);
                    //            }
                    //        }
                    //    }
                    //}
                }
            }
        }
        SafeRelease(pHDFaceFrame);
    }

    //if (m_IsSavedBtnPressed)
    //{
    //    cv::putText(m_faceMat, "Saving Faces...\nClick again to stop saving...", cv::Point(150, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0f, cv::Scalar (0,0,0), 2, CV_AA);
    //    m_FFFaceComponent->SaveIndices();
    //}
    //else
    //{
    //    cv::putText(m_faceMat, "Not saving Faces...", cv::Point(150, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0f, cv::Scalar(0, 0, 0), 2, CV_AA);
    //}
    //
    //cv::resize(m_colorMat, m_faceMat, cv::Size(), 0.5, 0.5);
    //cv::imshow("Lynnette FYP Face Detector", m_faceMat);

    return S_OK;
}

void AJKinect::ProcessCapturing()
{
    float audioBuffer[cAudioBufferLength];
    DWORD cbRead = 0;

    // S_OK will be returned when cbRead == sizeof(audioBuffer).
    // E_PENDING will be returned when cbRead < sizeof(audioBuffer).
    // For both return codes we will continue to process the audio written into the buffer.
    HRESULT hr = m_pAudioStream->Read((void *)audioBuffer, sizeof(audioBuffer), &cbRead);

    if (FAILED(hr) && hr != E_PENDING)
    {
        return;
        //SetStatusMessage(L"Failed to read from audio stream.");
    }

    if (!m_IsRecording)
    {
        return;
    }

    char convertedBuffer[cAudioBufferLength * 2];
    for (unsigned i = 0 ; i < cAudioBufferLength ; ++i)
    {
        short value = (short)(audioBuffer[i] * 32767.0);
        short* buffer = (short*)&convertedBuffer[i * 2];
        *buffer = value;
    }

    m_AudioRecorder.WriteBuffer(convertedBuffer, cbRead / 2);

    // grab active face
    AJFaceComponent* activeFace = nullptr;
    size_t objSize = m_FaceRecorder.m_ObjList.size();
    for (size_t i = 0; i < objSize; ++i)
    {
        AJFaceComponent* faceComp = m_FaceRecorder.m_ObjList[i]->FindComponent<AJFaceComponent>();
        if (   !faceComp
            || !faceComp->IsTracked())
            continue;

        if (faceComp->GetActive())
        {
            activeFace = faceComp;
            break;
        }
    }

    if (activeFace != nullptr)
    {
        const SimpleVertex* data = activeFace->GetVertexData();
        m_FaceRecorder.WriteBuffer(data, m_KinectFaceVertexCount);
    }
    else
    {
        // fail to capture
        m_FaceRecorder.WriteBuffer(nullptr, m_KinectFaceVertexCount);
    }
    // saveeeeeee the sound
}
