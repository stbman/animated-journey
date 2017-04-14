
import aj_globalvar
import numpy as np

def EvaluateFaceFrame(startIndex, endIndex):

    baseBuffer = []
    targetBuffer = []
    amplitudeBuffer = []
    generatedBuffer = []

    for iter in xrange(startIndex, endIndex):
        baseBuffer.append(aj_globalvar.g_originalFaceData[iter][0])
        targetBuffer.append(aj_globalvar.g_generatedFaceData[iter])
        amplitudeBuffer.append(aj_globalvar.g_generatedFaceAmplitude[iter])

    amplitudeData = np.abs(amplitudeBuffer)
    sortedData = np.sort(amplitudeData)
    clampIndex = int(len(sortedData) * 0.1)
    clampedVolume = sortedData[clampIndex]
    peakIndex = int(len(sortedData) * 0.9)
    peakVolume = sortedData[peakIndex]
    anchorFrame = aj_globalvar.g_originalFaceData[0][0].copy();

    targetGenerated = targetBuffer[0].copy()
    frameCount = len(baseBuffer)
    vertexCount = len(targetGenerated)
    for iter in xrange(frameCount):
        currentGenerated = targetBuffer[iter].copy()
        ratioMultiplier = 0.5
        if amplitudeData[iter] <= clampedVolume:
            currentGenerated = anchorFrame;
            ratioMultiplier = 1.0

        if amplitudeData[iter] >= peakVolume:
            ratioMultiplier = 1.0

        for j in xrange(len(aj_globalvar.g_InterpolationIndex)):
            i = aj_globalvar.g_InterpolationIndex[j]
            ratio = aj_globalvar.g_InterpolationData[i] * aj_globalvar.g_InterpolationData[i]
            ratio = ratio * ratioMultiplier
            inverseRatio = 1.0 - ratio
        
            targetGenerated[i][0] = (currentGenerated[i][0] * ratio) + (targetGenerated[i][0] * inverseRatio)
            targetGenerated[i][1] = (currentGenerated[i][1] * ratio) + (targetGenerated[i][1] * inverseRatio)
            targetGenerated[i][2] = (currentGenerated[i][2] * ratio) + (targetGenerated[i][2] * inverseRatio)

            generatedBuffer.append(targetGenerated)

    # compare rate of change with all points! and use the generated mask!

    baseBufferAvg = baseBuffer[0].copy()
    generatedBufferAvg = generatedBuffer[0].copy()
    for x in xrange(1, frameCount):
        for y in xrange(vertexCount):
            baseBufferAvg[y][0] += baseBuffer[x][y][0]
            baseBufferAvg[y][1] += baseBuffer[x][y][1]
            baseBufferAvg[y][2] += baseBuffer[x][y][2]
            generatedBufferAvg[y][0] += generatedBuffer[x][y][0]
            generatedBufferAvg[y][1] += generatedBuffer[x][y][1]
            generatedBufferAvg[y][2] += generatedBuffer[x][y][2]

    
    for y in xrange(vertexCount):
        baseBufferAvg[y][0] /= frameCount;
        baseBufferAvg[y][1] /= frameCount;
        baseBufferAvg[y][2] /= frameCount;
        generatedBufferAvg[y][0] /= frameCount;
        generatedBufferAvg[y][1] /= frameCount;
        generatedBufferAvg[y][2] /= frameCount;

    baseFrameDelta = []
    generatedFrameDelta = []
    for x in xrange(frameCount):
        currentDelta = 0.0
        for y in xrange(vertexCount):
            importanceWeight = aj_globalvar.g_InterpolationData[i] * aj_globalvar.g_InterpolationData[i]
           
            baseVertexDeltaX = np.abs(baseBuffer[x][y][0] - baseBufferAvg[y][0]) * importanceWeight
            baseVertexDeltaY = np.abs(baseBuffer[x][y][1] - baseBufferAvg[y][1]) * importanceWeight
            baseVertexDeltaZ = np.abs(baseBuffer[x][y][2] - baseBufferAvg[y][2]) * importanceWeight

            generatedVertexDeltaX = np.abs(generatedBuffer[x][y][0] - generatedBufferAvg[y][0]) * importanceWeight
            generatedVertexDeltaY = np.abs(generatedBuffer[x][y][1] - generatedBufferAvg[y][1]) * importanceWeight
            generatedVertexDeltaZ = np.abs(generatedBuffer[x][y][2] - generatedBufferAvg[y][2]) * importanceWeight

            baseFrameDelta.append(baseVertexDeltaX + baseVertexDeltaY + baseVertexDeltaZ)
            generatedFrameDelta.append(generatedVertexDeltaX + generatedVertexDeltaY + generatedVertexDeltaZ)

    baseCompare = []
    generatedCompare = []
    for iter in xrange(0, frameCount - 1):
        if baseFrameDelta[iter] > baseFrameDelta[iter+1]:
            baseCompare.append(1)
        elif baseFrameDelta[iter] < baseFrameDelta[iter+1]:
            baseCompare.append(-1)
        else:
            baseCompare.append(0)

        if generatedFrameDelta[iter] > generatedFrameDelta[iter+1]:
            generatedCompare.append(1)
        elif generatedFrameDelta[iter] < generatedFrameDelta[iter+1]:
            generatedCompare.append(-1)
        else:
            generatedCompare.append(0)

    baseResult = np.sum(baseCompare)
    generatedResult = np.sum(generatedCompare)
    resultDelta = np.abs(generatedResult - baseResult)
    #result = generatedResult / baseResult

    compareLen = len(baseCompare)
    mismatchRatio = float(resultDelta) / float(compareLen)

    baseZeroCrossings      = len(np.where(np.diff(np.sign(baseCompare)))[0])
    generatedZeroCrossings = len(np.where(np.diff(np.sign(generatedCompare)))[0])

    deltaZeroCrossing = np.abs(generatedZeroCrossings - baseZeroCrossings)
    zeroCrossingRatio = float(deltaZeroCrossing) / float(baseZeroCrossings)

    print "Mismatch Ratio: " + str(mismatchRatio) + " frame mismatch: " + str(resultDelta)
    print "Base zero crossing: " + str(baseZeroCrossings) + " Generated zero crossing: " + str(generatedZeroCrossings)
    print "Zero Crossing Ratio: " + str(zeroCrossingRatio)
    return mismatchRatio, resultDelta, baseZeroCrossings, generatedZeroCrossings, zeroCrossingRatio
