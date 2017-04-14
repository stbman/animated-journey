# -*- coding: utf-8 -*

"""
Created on Sun Jun 05 19:21:52 2016

For ploting wav file and finding peaks

WARNING: This file is deprecated

@author: Adrian Lim Xuan Wei
"""

import matplotlib.pyplot as plt

from scipy.fftpack import fft
from scipy.io import wavfile 
from scipy import signal
import numpy

from aj_detectpeaks import detect_peaks
import aj_globalvar
from aj_vecmath import *

import aj_binaryreader

import subprocess, os

def GenerateInterpolationMap():
    rootDir = "./EmotionAdrian/"
    fileEndWith = "face_formatted.apf"
    resultPath = rootDir + "interpolationMap.csv"

    mergeData = []
    deltaData = []

    dataCount = 0;

    for subdir, dirs, files in os.walk(rootDir):
        for f in files:
            if f.endswith(fileEndWith):
                fileName = os.path.join(subdir, f)
                print "Adding Average: " + fileName
                data = readBinaryFile(fileName)

                if len(mergeData) == 0 :
                    mergeData = data[0].copy()
                    for i in xrange(len(mergeData)):
                        mergeData[i]['posx'] = 0
                        mergeData[i]['posy'] = 0
                        mergeData[i]['posz'] = 0
                    
                for i in xrange(len(data)):
                    dataCount += 1
                    for j in xrange(len(data[i])):
                        mergeData[j]['posx'] += data[i][j]['posx']
                        mergeData[j]['posy'] += data[i][j]['posy']
                        mergeData[j]['posz'] += data[i][j]['posz']
                        #v = numpy.empty_like(mergeData)
                        #for col in mergeData.dtype.names:
                        #    v[col] = mergeData[col] + data[col]
                               
                        #mergeData = v.copy()

                    #for i in xrange(len(data)):
                    #    for j in xrange(len(data[i])):
                    #        temp = numpy.array(data[i][j])
                    #        mergeData[j] += numpy.add(mergeData[j], temp)
                    #    dataCount += 1

                    # merge all data into 1 face
                    # divide data by total number of face

    print "Generating Averaged Data"
    for i in xrange(len(mergeData)):
        mergeData[i]['posx'] /= dataCount
        mergeData[i]['posy'] /= dataCount
        mergeData[i]['posz'] /= dataCount

    for subdir, dirs, files in os.walk(rootDir):
        for f in files:
            if f.endswith(fileEndWith):
                fileName = os.path.join(subdir, f)
                print "Adding Delta: " + fileName
                data = readBinaryFile(fileName)

                if len(deltaData) == 0 :
                    deltaData = data[0].copy()
                    for i in xrange(len(deltaData)):
                        deltaData[i]['posx'] = 0
                        deltaData[i]['posy'] = 0
                        deltaData[i]['posz'] = 0
                    
                for i in xrange(len(data)):
                    dataCount += 1
                    for j in xrange(len(data[i])):
                        deltaData[j]['posx'] += abs(data[i][j]['posx'] - mergeData[j]['posx'])
                        deltaData[j]['posy'] += abs(data[i][j]['posy'] - mergeData[j]['posy'])
                        deltaData[j]['posz'] += abs(data[i][j]['posz'] - mergeData[j]['posz'])

    print "Generating Delta Data"
    deltaArray = numpy.empty(len(deltaData))
    for i in xrange(len(deltaArray)):
        posXSq = deltaData[i]['posx'] * deltaData[i]['posx']
        posYSq = deltaData[i]['posy'] * deltaData[i]['posy']
        posZSq = deltaData[i]['posz'] * deltaData[i]['posz']
        sqroot = sqrt(posXSq + posYSq + posZSq)
        deltaArray[i] = sqroot

        #deltaArray[i]  = (deltaData[i]['posx'] * deltaData[i]['posx'])
        #deltaArray[i] += (deltaData[i]['posy'] * deltaData[i]['posy'])
        #deltaArray[i] += (deltaData[i]['posz'] * deltaData[i]['posz'])
        #deltaArray[i] = numpy.sqrt(deltaArray[i])

    highestDelta = max(deltaArray)
    lowestDelta = min(deltaArray)
    convertDelta = highestDelta - lowestDelta
    #for i in xrange(len(deltaArray)):
    #    if deltaArray[i] > highestDelta:
    #        highestDelta = deltaArray[i]

    for i in xrange(len(deltaArray)):
        deltaArray[i] = (deltaArray[i] - lowestDelta) / convertDelta

    # deltaData will contain the ratio for interpolation
    # will need to do histogram normalization?




    # loop at face.apf file used currently
    # generate an average face from all the faces
    # generate weight of each vertex
    # save weight to file

    # use result as weight for interpolation between previous and next frame
    numpy.savetxt(resultPath, deltaArray, delimiter=",")
    return deltaArray

def convertAllAPFFiles():
    rootDir = "./EmotionAdrian/"
    anchorFaceReader = aj_binaryreader.BinaryReader(rootDir + "anchor.apf")
    anchorFace = anchorFaceReader.rearrangeVertices().copy()

    for subdir, dirs, files in os.walk(rootDir):
        for f in files:
            if f.endswith("face.apf"):
                fileName = os.path.join(subdir, f)
                if not "_formatted" in fileName:
                    print "Converting: " + fileName
                    #newFileName = str(fileNameArray[0]) + '_formatted.apf'
                    idx = fileName.index('.apf')
                    newFileName = fileName[:idx] + '_formatted.apf'
                    # read original file from fileName
                    data = readBinaryFile(fileName)
                    #data = data[0]
                    processedData = normalizeData(data, anchorFace)
                    # write out to new file at newFileName
                    writeToBinaryFile(newFileName, data)

def normalizeData(data, anchor):
    anchorLeft  = 469
    anchorRight = 1117
    anchorTop   = 28
    anchorBtm   = 18

    anchorMtx = m44Identity()
    anchorMtx[:,0] = v4FromVertex(anchor[anchorRight]) - v4FromVertex(anchor[anchorBtm])
    anchorMtx[:,1] = v4FromVertex(anchor[anchorLeft])  - v4FromVertex(anchor[anchorBtm])
    anchorMtx[:,2] = v4FromVertex(anchor[anchorTop])   - v4FromVertex(anchor[anchorBtm])


    # iterate all frame
    for i in xrange(len(data)):
        vertexData = data[i]
        
        tranMX = m44Identity()
        tranMX[:,3] -= v4FromVertex(vertexData[anchorBtm])

        localMtx = m44Identity()
        localMtx[:,0] =   v4FromVertex(vertexData[anchorRight]) - v4FromVertex(vertexData[anchorBtm])
        localMtx[:,1] =   v4FromVertex(vertexData[anchorLeft])  - v4FromVertex(vertexData[anchorBtm])
        localMtx[:,2] =   v4FromVertex(vertexData[anchorTop])   - v4FromVertex(vertexData[anchorBtm])

        localMtx = numpy.linalg.inv(localMtx)
        resultMx = m44_mul_m44(localMtx, tranMX)
        resultMx = m44_mul_m44(anchorMtx, resultMx)

        # normalize all data
        for j in xrange(len(vertexData)):
            result = v4FromVertex(vertexData[j])
            result[3] = 1.0
            normalized = resultMx.dot(result)
            vertexData[j][0] = normalized[0] # pos.x
            vertexData[j][1] = normalized[1] # pos.y
            vertexData[j][2] = normalized[2] # pos.z
            vertexData[j][3] = anchor[j][3] # uv.u
            vertexData[j][4] = anchor[j][4] # uv.v

    return data

def readBinaryFile(fileName):
    frame_chunk = 1347
    
    f = open(fileName, 'rb')
        
    dtype = numpy.dtype([
        ("posx", numpy.float32),
        ("posy", numpy.float32),
        ("posz", numpy.float32),
        ("texu", numpy.float32),
        ("texv", numpy.float32),
        ("norx", numpy.float32),
        ("nory", numpy.float32),
        ("norz", numpy.float32)
    ])
    
    data = numpy.fromfile(f, dtype=dtype)
    
    #data['posx'] = data['posx']
    #data['posy'] = data['posy']
    #data['posz'] = data['posz']
    
    num_frames = len(data) / frame_chunk
    data_split = numpy.split(data, num_frames)
    return data_split

def writeToBinaryFile(newFileName, data):
    f = open(newFileName, 'wb')
    data = numpy.array(data).tostring()
    f.write(data)

'''
To convert Kinect generated .wav files 
to .wav files that can be used by scipy
using ffmpeg
'''
def convertAllWavFiles():
    rootDir = "./EmotionAdrian010117"
    for subdir, dirs, files in os.walk(rootDir):
        for f in files:
            if f.endswith(".wav"):
                fileName = os.path.join(subdir, f)
                fileNameArray = fileName.split('.wav')
                newFileName = str(fileNameArray[0]) + '2.wav'
                subprocess.call('./ffmpeg -i ' + fileName + ' ' + newFileName)

def processAllAudioData():
    rootDir = "./sound/textreadings/"
    for subdir, dirs, files in os.walk(rootDir):
        for d in dirs: 
            subdirName = os.path.join(subdir, d)
            recordingFile = subdirName + "/recording2.wav"
            faceFile = subdirName + "/faceProcessed.apf"
            audioProcessing(recordingFile, faceFile)

'''
Detect peaks from wav audio sample
Return numpy array indices of peaks
When show=True, the plot will show
'''
def detectPeaksFromWavFile(soundFile, show=True):    
    #soundFile = "./sound/textreadings/4/recording2.wav"
    
    rate, data = wavfile.read(soundFile)
    
    ind = detect_peaks(data, mph=max(data)/8, mpd=1000, show=show)
    
    return ind

#plt.plot(data)
#plt.show()

    
'''
Takes in numpy array
Return numpy array indices of peaks
When show=True, plot will show
- max peak height: max(data)/8
- min peak distance: 1000
'''
def detectPeaksFromNumpyArr(data, show=True):
    #halfDataTrimmed = halfData[2:]
    ind = detect_peaks(data, mph=max(data)/8, mpd=1000, show=show)
    return ind

'''
Plots data from wav file 

Inumpyut: numpy array of wav file data, numpy array of peakTuples
Output: matplotlib plot of wav file start end indicated with vertical lines
'''
def plotSignalStartEnd(data, peakTuples):
    plt.plot(data)
    
    for pt in peakTuples: 
        plt.axvline(x=pt[0], color='r')
        plt.axvline(x=pt[1], color='r')
        
    plt.show()

'''
Autocorrelates signal

Inumpyut: numpy array signal
Output: autocorrelated numpy array signal
'''
def autocorrelation(signal):
    result = numpy.correlate(signal, signal, mode='full')
    return result[result.size/2:]

'''
Get window size of a signal, 
Determined by update rate 
'''
def getWindowSize():
    windowSize = aj_globalvar.framerate * aj_globalvar.updaterate

    return int(windowSize)

'''
Inumpyut: 
- point: float (index of peak)
- halfWindow: windowsize/2
- data: numpy array of audio data
'''
def findFundamentalFreqOfPoint(point, halfWindow, data):
    import scipy

    lengthOfData = len(data)

    left = point - halfWindow
    if left < 0:
        left = 0
    right = point + halfWindow
    if right > lengthOfData:
        right = lengthOfData

    truncatedData = data[left:right]
    autoCorrData = autocorrelation(truncatedData)

    fftData = scipy.fftpack.fft(autoCorrData)
    halfFFTData = numpy.abs(fftData[:len(autoCorrData)/2+1])    # FFT is symmetrical
    fundamentalFreq = numpy.argmax(halfFFTData)

    return fundamentalFreq

'''
Find closest note

Output: note: string
'''

def findNoteOfPeak(fundamentalFreq):
    # Find nearest float 
    min = 999999
    key = None

    for k, v in aj_globalvar.fundamentalFreqDict.iteritems():
        diff = abs(v-fundamentalFreq)
        if diff < min:
            min = diff
            key = k

    return key

def getFaceInfo(peakIdx, numFaces):
    faceRatio = numpy.floor(int(peakIdx)/ numFaces)

    return int(faceRatio)

'''
Inumpyut face points to dictionary

Inumpyut: 
- note: string
- amp: amplitude, float 
- facePoints: numpy array 

'''
def inumpyutToDict(note, amp, facePoints):
    if note in aj_globalvar.freqToFaceDict:
        # Assuming amplitudes are unique cuz floats should be unique...
        aj_globalvar.freqToFaceDict[note][amp] = facePoints
    else:
        aj_globalvar.freqToFaceDict[note] = {}
        aj_globalvar.freqToFaceDict[note][amp] = facePoints

'''
Inumpyut: 
- audio filename: string
- face filename: string
Output: None (add to dictionary)

1. Read fundamental freq json file 
2. Find peaks from audio signal 
3. Perform gradient descent to get (left, peak, right) of each peak
4. Find fundamental frequency of peak 
5. Find note of peak 
6. Add to dictionary (freqToFaceDict) face data
7. Divide an average for each freq 
'''
def audioProcessing(audioFile, faceFile):
    # Process audio
    import scipy
    rate, audioData = wavfile.read(audioFile)
    peaks = detectPeaksFromNumpyArr(audioData, show=False)
    #windowSize = getWindowSize()
    windowSize = 256
    halfWindow = int(windowSize/2)
    lengthOfData = len(audioData)

    # Process faces
    import aj_binaryreader
    br = aj_binaryreader.BinaryReader(faceFile)
    numFaces = br.getLengthOfData()

    for i in xrange(len(peaks)):
        currPeak = peaks[i]
    
        delta = 3
        top = currPeak
        
        # Going down to the left 
        leftPoint = top - delta
        diffLeft = audioData[top] - audioData[leftPoint]
        while diffLeft > 0:
            if audioData[leftPoint] < 0:
                break
            
            top = leftPoint
            leftPoint = top - delta
            diffLeft = audioData[top] - audioData[leftPoint]
            
        # Going down to the right 
        rightPoint = top + delta
        diffRight = audioData[top] - audioData[rightPoint]
        while diffRight < 0:
            if audioData[rightPoint] < 0: 
                break
            
            top = rightPoint
            rightPoint = top + delta
            diffRight = audioData[top] - audioData[rightPoint]

        # Amplitude of peaks
        leftAmp = abs(audioData[leftPoint])
        currAmp = abs(audioData[currPeak])
        rightAmp = abs(audioData[rightPoint])
            
        # Find fundamental freq of peaks
        leftFreq = findFundamentalFreqOfPoint(leftPoint, halfWindow, audioData)
        peakFreq = findFundamentalFreqOfPoint(currPeak, halfWindow, audioData)
        rightFreq = findFundamentalFreqOfPoint(rightPoint, halfWindow, audioData)

        # Find note of peaks
        leftNote = findNoteOfPeak(leftFreq)
        currNote = findNoteOfPeak(peakFreq)
        rightNote = findNoteOfPeak(rightFreq)

        # Get face index
        leftFaceIdx = getFaceInfo(numpy.floor(leftPoint/lengthOfData), numFaces)
        currFaceIdx = getFaceInfo(numpy.floor(currPeak/lengthOfData), numFaces)
        rightFaceIdx = getFaceInfo(numpy.floor(rightPoint/lengthOfData), numFaces)

        #print leftPoint, leftFaceIdx

        # Get face points
        leftFacePoints = br.getRearrangedVertices(leftFaceIdx)
        currFacePoints = br.getRearrangedVertices(currFaceIdx)
        rightFacePoints = br.getRearrangedVertices(rightFaceIdx)

        # Inumpyut to dict
        # Format: {'ff': {amp: [points]}}
        inumpyutToDict(leftNote, leftAmp, leftFacePoints)
        inumpyutToDict(currNote, currAmp, currFacePoints)
        inumpyutToDict(rightNote, rightAmp, rightFacePoints)

    # Add a divide average for global dict 
    # for k, v in aj_globalvar.freqToFaceDict.iteritems():
    #     avg = getAveFaceVals(k)
    #     aj_globalvar.freqToFaceDict[k]['avg'] = avg
    
    return aj_globalvar.freqToFaceDict
'''
Note: Can only be called after dictionary popularised

Inumpyut:
- note: string 

Output: 
- avg: numpy array
average face points for the note 
'''
def getAveFaceVals(note):
    d = aj_globalvar.freqToFaceDict[note]

    avg = None
    count = 0

    for val in d.values():
        v = val[0]

        if avg is None:
            avg = v
        else: 
            for i in xrange(len(v)):
                avg[i][0] = avg[i][0] + v[i][0]
                avg[i][1] = avg[i][1] + v[i][1]
                avg[i][2] = avg[i][2] + v[i][2]
                avg[i][3] = avg[i][3] + v[i][3]
                avg[i][4] = avg[i][4] + v[i][4]

        count += 1

    # Divide the average
    for i in xrange(len(avg)):
        avg[i][0] = avg[i][0] / count
        avg[i][1] = avg[i][1] / count
        avg[i][2] = avg[i][2] / count
        avg[i][3] = avg[i][3] / count
        avg[i][4] = avg[i][4] / count

    return avg

'''
Get face points arrays from sound file 

1. Read sound file 
2. Detect peaks
3. Get (start peak end) of peaks 
4. Get fundamental freq of peaks 
5. Get average faces of fundamental freq of peaks
'''
def getFacesFromSound(soundFile):
    rate, audioData = wavfile.read(soundFile)
    peaks = detectPeaksFromNumpyArr(audioData, sound=False)

    facesList = []

    # Get start peak end of peaks 

    # Get fundamental freq 

    return facesList

if __name__ == "__main__":
    soundFile = "./sound/textreadings/1/recording2.wav"
    #rate, data = wavfile.read(soundFile)
    #peaks = detectPeaksFromWavFile(soundFile, show=False)
    #peakTuples = getPeakStartEnd(peaks)
    #plotSignalStartEnd(data, peakTuples)
    #calculateFreqOfPeaks(data, peaks)

    #import aj_texttospeech
    #subData = data[76140:85201]
    #subData = data[36995:37004]
    #aj_texttospeech.playWavFileFromArr(data)

    #calculateSpectrogram(data)

    faceFile = "./sound/textreadings/1/faceProcessed.apf"
    #ffd = audioProcessing(soundFile, faceFile)
    #avg = getAveFaceVals('Cis2')

    #convertAllWavFiles()

    #processAllAudioData()
    #convertAllAPFFiles()
    #convertAllWavFiles()
    GenerateInterpolationMap()
    x = 10