
# -*- coding: utf-8 -*-
"""
Find longest common subsequence between speech data objects

@author: Adrian Lim Xuan Wei
"""

from scipy.io import wavfile
import numpy
from python_speech_features import mfcc, logfbank

import aj_speechnote
import aj_globalvar
import aj_soundprocessorhelper
import aj_speechprocessor

from aj_plotaudio import PlotAudio

class GraphProcessor(aj_speechprocessor.Speechprocessor):

    def __init__(self):
        self.allSpeechData = aj_globalvar.allSpeechNotesArray
        self.intervalBetweenPeak = 0.20 # secs
        self.intervalBeforePeak = 0.60

    '''
    1. Find longest common subsequence between speechDataSegment 
    and allSpeechData notes
    2. Use the speechData of closest match to return
    - face datas
    - notes
    - amplitude
    '''
    def getClosestMatch(self, soundFile):
        self.speechData = []

        self.soundFile = soundFile
        self.readSoundFile()

        self.processSpeech()

        # array of sd objects
        matchedObjects = []

        i=0

        print 'Number of Segments: ' + str(len(self.speechData))

        for segment in self.speechData:
            print 'Segment ' + str(i)
            i += 1
            notesInSegment = segment['notes']

        #notesData = speechDataSegment.speechData['notes']
        #self.lengthOfSegment = len(notesData)
        
            maxLength = -1
            maxLengthSpeechData = {}

            for sd in self.allSpeechData:
                targetNotesArray = sd['notes']
                #splicedNoteArray = notesArray[:self.lengthOfSegment]
                lengthLongestSubsequence = lcs(targetNotesArray, notesInSegment)
                #lengthLongestSubsequence = len(longestCommonSubsequence)
                sd['length'] = lengthLongestSubsequence

                if sd['length'] > maxLength:
                    maxLength = sd['length']
                    maxLengthSpeechData = sd

            match = {}
            match['notes'] = maxLengthSpeechData['notes'][-1]
            match['faceData'] = maxLengthSpeechData['faceData']
            match['audio'] = maxLengthSpeechData['audio'][-1]
            matchedObjects.append(match)

        x = 10
        return matchedObjects

    def processSpeech(self):
        for i in xrange(0, self.lengthOfData, self.audioWindowIndices):
            speechData = {'notes': [], 'audio': [], 'faceData': []}

            if i < self.audioBeforePeakWindowIndices:
                start = 0
            else:
                start = self.audioBeforePeakWindowIndices

            for j in xrange(start, i+1, self.audioWindowIndices):
                amplitude = self.audioData[j]
                fundamentalFreq = self.findFundamentalFreqOfPoint(j)
                note = self.findNoteOfOnePeak(fundamentalFreq)

                speechData['notes'].append(note.name)
                speechData['audio'].append(amplitude)

            self.speechData.append(speechData)

        print 'Speech processed'


def lcs(a, b):
    lengths = [[0 for j in range(len(b)+1)] for i in range(len(a)+1)]
    # row 0 and column 0 are initialized to 0 already
    for i, x in enumerate(a):
        for j, y in enumerate(b):
            if x == y:
                lengths[i+1][j+1] = lengths[i][j] + 1
            else:
                lengths[i+1][j+1] = max(lengths[i+1][j], lengths[i][j+1])
    # read the substring out from the matrix
    result = []
    x, y = len(a), len(b)
    while x != 0 and y != 0:
        if lengths[x][y] == lengths[x-1][y]:
            x -= 1
        elif lengths[x][y] == lengths[x][y-1]:
            y -= 1
        else:
            assert a[x-1] == b[y-1]
            result.insert(0, a[x-1])
            #result = a[x-1] + result
            x -= 1
            y -= 1
    return len(result)

'''
Sound file: wav file name
Time: in seconds
'''
def GetFaceFrame(framerate, audioData, time, enableThresholding):
    #framerate , audioData = wavfile.read(soundFile)
    audioIndex = getIndexAtTime(time)

    return GetFaceFrameFromData(framerate, audioData, audioIndex, enableThresholding)

def GetFaceFrameFromData(framerate, audioData, audioIndex, audioThreshold=None, plot=False):
    # Normalise audio
    #audioData = normaliseAudio(audioData)

    if audioThreshold is not None:
        threshold = audioThreshold
        
    #if plot is not None:
    #    thresholdToPlot = audioThreshold or 0.0
    #    PlotAudio(audioData, thresholdToPlot, 'received_' + str(plot))

    # go to time
    intervalBetweenPeak = aj_globalvar.audioWindow
    indicesBetweenPeak = getIndexAtTime(intervalBetweenPeak)

    #print time, len(audioData), audioIndex

    # create sliding window? kernel? base on that time - [ (time - 5frame worth) to time]
    speechData = {'notes': [], 'audio': [], 'faceData': []}

    if audioIndex < 0:
        start = 0
    else:
        start = audioIndex - (5 * intervalBetweenPeak)

    start = int (start)

    if audioIndex > len(audioData):
        end = len(audioData)
    else:
        end = audioIndex

    print 'Audio Index Length: ' + str(len(audioData))

    # get the string/sequence from this window
    for i in xrange(start, end, indicesBetweenPeak):
        #print start, end, i
        amplitude = audioData[i]
        fundamentalFreq = aj_soundprocessorhelper.findFundamentalFreqOfPoint(audioData, i)
        note = aj_soundprocessorhelper.findNoteOfOnePeak(fundamentalFreq)

        speechData['notes'].append(note.name)
        speechData['audio'].append(amplitude)

    #truncatedAudio = audioData[start:end]
    #print 'Truncated audio length: ' + str(len(truncatedAudio))
    #mfccFeatures = mfcc(audioData)[0]
    #speechData['mfcc'] = mfccFeatures

    return GetFaceFrameForSequence(speechData, audioThreshold)

def normaliseAudio(audioData):
    MAX_WAV16_AMP = 32767.0  # = 2**15-1  # because wav16 is signed
    #audioData = audioData / MAX_WAV16_AMP
    maxAudio = max(audioData)
    minAudio = min(audioData)

    rootMeanSquare = numpy.sqrt(numpy.mean(numpy.square(audioData)))
    dividedAudio = audioData / rootMeanSquare
    clampedAudio = numpy.clip(dividedAudio, minAudio, maxAudio)

    return clampedAudio

def GetFaceFrameForSequence(speechData, threshold):
    notesArray = speechData['notes']

    if (numpy.abs(speechData['audio']) < threshold):
        return [], notesArray

    # Get Face Frame for the sequence

    allSpeechNotesArrayLocal = aj_globalvar.allSpeechNotesArray

    for sd in allSpeechNotesArrayLocal:
        targetNotesArray = sd['notes'][:len(notesArray)]
        lengthLongestSubsequence = lcs(targetNotesArray, notesArray)
        sd['length'] = lengthLongestSubsequence

        #if lengthLongestSubsequence > maxLength:
        #    maxLength = lengthLongestSubsequence
        #    maxLengthSpeechData = sd

    #sortedSpeechNotesArray = sorted(allSpeechNotesArrayLocal, key=lambda k: k['length'], reverse=True)
    #longestLength = sortedSpeechNotesArray[0]['length']
    longestLength = max(allSpeechNotesArrayLocal, key=lambda x: x['length'])['length']
    longestLengthList = filter(lambda sd: sd['length'] == longestLength, allSpeechNotesArrayLocal)

    if len(longestLengthList) == 1:
        return longestLengthList['faceData'][0], notesArray
    else: 
        #closestFaceMatch = matchFaceFramesWithMfcc(longestLengthList, speechData)
        closestFaceMatch = matchFaceFramesWithAmplitude(longestLengthList, speechData)
        return closestFaceMatch['faceData'][0], notesArray

    #print "HAHA" + str(len(maxLengthSpeechData['faceData'][0]))
    #return maxLengthSpeechData['faceData'][0]

'''
longestLengthList = [{'notes': [Enum.Note], 'audio': [float], faceData: []}]

use dot product of MFCC vectors
'''
def matchFaceFramesWithMfcc(longestLengthList, speechData):
    targetAudioMfcc = speechData['mfcc']

    for sd in longestLengthList:
        sdMfcc = sd['mfcc']
        dotProduct = numpy.dot(targetAudioMfcc, sdMfcc)
        sd['mfccDistance'] = dotProduct

    sortestLengthData = min(longestLengthList, key=lambda x: x['mfccDistance'])

    return sortestLengthData

'''
longestLengthList = [{'notes': [Enum.Note], 'audio': [float], faceData: []}]

use manhattan distance
'''
def matchFaceFramesWithAmplitude(longestLengthList, speechData):
    targetAudio = speechData['audio']

    for sd in longestLengthList:
        sdAudio = sd['audio'][:len(targetAudio)]
        manhattanDistance = numpy.sum(numpy.abs(numpy.subtract(targetAudio, sdAudio)))
        sd['audioLength'] = manhattanDistance

    #longestLengthList = sorted(longestLengthList, key=lambda k: k['audioLength'], reverse=False)
    sortestLengthData = min(longestLengthList, key=lambda x: x['audioLength'])

    return sortestLengthData

def getIndexAtTime(time):
    index = float (aj_globalvar.framerate) * time
    return int (index)

if __name__ == "__main__":
    import os
    rootDir = aj_globalvar.rootDir

    import aj_speechprocessor

    '''
    for subdir, dirs, files in os.walk(rootDir):
        recordingFile = os.path.join(subdir, aj_globalvar.recordingFile)
        faceFile = os.path.join(subdir, aj_globalvar.faceFile)
        #print recordingFile, faceFile
        aj_speechprocessor.Speechprocessor(soundFile, faceFile)
    '''
    aj_speechprocessor.processAllAudioData()

    # Testing input file 
    soundFile = "./sound/recording/adrianTest.wav"
    faceFile = "./Emotion/Anger/1/face.apf"
    #sp = aj_speechprocessor.Speechprocessor(soundFile, faceFile, True)

    #graphProcessor = GraphProcessor()
    #speechData = graphProcessor.getClosestMatch(soundFile)
    # to get face data
    #faceData = [sd['faceData'] for sd in speechData]

    faceFrame = GetFaceFrame(soundFile, 1.20)
    x = 10