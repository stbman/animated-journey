
# -*- coding: utf-8 -*-
"""
@author: Adrian Lim Xuan Wei
"""

import scipy
from scipy.fftpack import fft
from scipy.io import wavfile

import numpy 

from aj_detectpeaks import detect_peaks
import aj_speechnote
import aj_binaryreader
import aj_globalvar

class Speechprocessor():
    
    def __init__(self, soundFile, faceFile):
        self.soundFile = soundFile
        self.faceFile = faceFile

        self.readSoundFile()
        self.initializeBinaryReader()
        self.processSpeech()

    def readSoundFile(self):
        aj_globalvar.framerate , self.audioData = wavfile.read(self.soundFile)
        self.numWindowIndices = self.getNumWindowIndices()
        self.halfNumWindowIndices = self.numWindowIndices / 2

        self.peaks = self.detectPeaksFromNumpyArr(self.audioData, show=False)

        #windowSize = self.getWindowSize()
        windowSize = 256
        self.halfWindow = int(windowSize/2)
        self.lengthOfData = len(self.audioData)

    def initializeBinaryReader(self):
        self.br = aj_binaryreader.BinaryReader(self.faceFile)
        self.numFaces = self.br.getLengthOfData()

    def processSpeech(self):
        for i in xrange(len(self.peaks)):
            currPeak = self.peaks[i]
            
            start, peak, end = self.findStartEnd(currPeak)

            #start, peak, end = self.performGradientDescent(currPeak)

            SpeechNote = self.constructSpeechNote(start, peak, end)

            startToMidFaceData = self.getFaceData(start, peak)
            midToEndFaceData = self.getFaceData(peak, end)
            #SpeechNote.setStartToMidFaceData(startToMidFaceData)
            #SpeechNote.setMidToEndFaceData(midToEndFaceData)

            faceData = startToMidFaceData + midToEndFaceData
            SpeechNote.setFaceData(faceData)
            SpeechNote.setMidFrameIndex(len(startToMidFaceData))

            aj_globalvar.allSpeechNotesArray.append(SpeechNote)

    def getNumWindowIndices(self):
        indices = float (aj_globalvar.windowWidth) * aj_globalvar.framerate
        # Floor because indices is a number
        return int (indices)

    def findStartEnd(self, currPeak):
        start = currPeak - self.halfNumWindowIndices
        if start < 0: 
            start = 0

        end = currPeak + self.halfNumWindowIndices
        if end > len(self.audioData):
            end = len(self.audioData)

        return start, currPeak, end

    '''
    Constructs most of speech note except for faceData 

    Input: index of start, peak, end from audio data: int
    Output: SpeechNote: SpeechNote
    '''
    def constructSpeechNote(self, start, peak, end):
        SpeechNote = aj_speechnote.speechNote()

        startToPeak, peakToEnd = self.calculateTimeDiff(start, peak, end)
        SpeechNote.setTimeDifference(startToPeak, peakToEnd)

        startAmp, peakAmp, endAmp = self.getAmplitudes(start, peak, end)
        SpeechNote.setAmp(startAmp, peakAmp, endAmp)

        startFreq, peakFreq, endFreq = self.findFundamentalFreq(start, peak, end)
            
        startNote, peakNote, endNote = self.findNotesOfPeaks(startFreq, peakFreq, endFreq)
        SpeechNote.setNotes(startNote, peakNote, endNote)

        return SpeechNote

    '''
    Calculate time difference in seconds 

    duration in sec = number of frames / frame rate 
    '''
    def calculateTimeDiff(self, start, peak, end):
        startToPeakIdxDiff = float(peak - start)
        peakToEndIdxDiff = float(end - peak)

        startToPeak  =  startToPeakIdxDiff / aj_globalvar.framerate
        peakToEnd = peakToEndIdxDiff / aj_globalvar.framerate

        return startToPeak, peakToEnd

    def getAmplitudes(self, start, peak, end):
        startAmp = self.audioData[start]
        peakAmp = self.audioData[peak]
        endAmp = self.audioData[end-1]

        return startAmp, peakAmp, endAmp

    '''
    Takes in numpy array
    Return numpy array indices of peaks
    When show=True, plot will show
    - max peak height: max(data)/8
    - min peak distance: 1000
    '''
    def detectPeaksFromNumpyArr(self, data, show=True):
        selfMph = max(data)/20
        selfMpd = selfMph * 5
        ind = detect_peaks(data, mph=selfMph, mpd=selfMpd, show=show)
        print "Max Peak: " + str(max(data)) + " MPH: " + str(selfMph) + " MPD: " + str(selfMpd) + " Detected Peaks: " + str(len(ind))
      #  ind = numpy.array([ 37001,  38033,  39080,  45397,  46650,  53194,  54285,  57313,
      #  61390,  63268,  73830,  74857,  76239,  80654,  88184,  89773,
      #  91363,  97580,  98602, 102199, 105670, 111351, 116845, 117880,
      # 131268, 134700, 135706, 143982, 145032, 147004, 148099, 149285,
      # 150694, 160156, 164267, 167733, 168791, 173883, 176229, 180942,
      # 191407, 194685, 198413])

        x = 10
        return ind

    '''
    Get window size of a signal, 
    Determined by update rate 
    '''
    def getWindowSize(self):
        windowSize = aj_globalvar.framerate * aj_globalvar.updaterate

        return int(windowSize)

    '''
    Perform gradient descent given a peak
    Input: index of current Peak: int
    Output: indices of start, peak, end: int
    '''
    def performGradientDescent(self, currPeak):
        delta = 3

        top = currPeak

        # Going down to the left 
        leftPoint = top - delta
        diffLeft = self.audioData[top] - self.audioData[leftPoint]
        while diffLeft > 0:
            if self.audioData[leftPoint] < 0:
                break
            
            top = leftPoint
            leftPoint = top - delta
            diffLeft = self.audioData[top] - self.audioData[leftPoint]
            
        # Going down to the right 
        rightPoint = top + delta
        diffRight = self.audioData[top] - self.audioData[rightPoint]
        while diffRight < 0:
            if self.audioData[rightPoint] < 0: 
                break
            
            top = rightPoint
            rightPoint = top + delta
            diffRight = self.audioData[top] - self.audioData[rightPoint]

        return leftPoint, currPeak, rightPoint
    '''
    Autocorrelates signal

    Input: numpy array signal
    Output: autocorrelated numpy array signal
    '''
    def autocorrelation(self, signal):
        result = numpy.correlate(signal, signal, mode='full')
        return result[result.size/2:]

    '''
    Find fundamental frequencies of start peak end
    Input: indices of start peak end: int
    Output: fundamental freq of start peak end: float
    '''
    def findFundamentalFreq(self, start, peak, end):
        startFreq = self.findFundamentalFreqOfPoint(start)
        peakFreq  = self.findFundamentalFreqOfPoint(peak)
        endFreq   = self.findFundamentalFreqOfPoint(end)

        return startFreq, peakFreq, endFreq

    '''
    Input: index in audio data array: int
    Output: fundamental frequency: float 
    '''
    def findFundamentalFreqOfPoint(self, point):
        left = point - self.halfWindow
        if left < 0:
            left = 0
        right = point + self.halfWindow
        if right > self.lengthOfData:
            right = self.lengthOfData

        truncatedData = self.audioData[left:right]
        autoCorrData = self.autocorrelation(truncatedData)

        fftData = scipy.fftpack.fft(autoCorrData)
        halfFFTData = numpy.abs(fftData[:len(autoCorrData)/2+1])    # FFT is symmetrical
        fundamentalFreq = numpy.argmax(halfFFTData)

        return fundamentalFreq

    def findNotesOfPeaks(self, leftFreq, peakFreq, rightFreq):
        leftNote  = self.findNoteOfOnePeak(leftFreq)
        peakNote  = self.findNoteOfOnePeak(peakFreq)
        rightNote = self.findNoteOfOnePeak(rightFreq)

        return leftNote, peakNote, rightNote

    '''
    Find nearest float 
    Input: fundamental frequency: float 
    Output: Note: enum
    '''
    def findNoteOfOnePeak(self, freq):
        note = aj_speechnote.getNoteFromFreq(freq)
        return note

    '''
    Get face data from the start to end 
    Input: indices of start and end in audio array
    Output: [faceData] from indices from start to end 
    Note: faceData rearranged according to indices 
    '''
    def getFaceData(self, start, end):
        startFaceRatio = self.getFaceRatio(start)
        endFaceRatio   = self.getFaceRatio(end)

        faceData = []

        for i in xrange(startFaceRatio, endFaceRatio + 1):
            rearrangedData = self.br.getRearrangedVertices(i)
            faceData.append(rearrangedData)

        return faceData

    '''
    Get the corresponding kinect face index from audio index
    Input: index in audio: int
    Output: index in Kinect: int
    '''
    def getFaceRatio(self, audioIdx):
        audioRatio = float(audioIdx)/ float(self.lengthOfData)
        faceRatio = audioRatio * self.numFaces

        return int(faceRatio)

def processAllAudioData():
    # Reset 
    aj_globalvar.allSpeechNotesArray = []
    
    import os
    rootDir = aj_globalvar.rootDir
    for subdir, dirs, files in os.walk(rootDir):
        for d in dirs: 
            subdirName = os.path.join(subdir, d)
            recordingFile = subdirName + aj_globalvar.recordingFile
            faceFile = subdirName + aj_globalvar.faceFile
            speechProcessor = Speechprocessor(recordingFile, faceFile)
    
    aj_globalvar.allSpeechNotesArray = aj_speechnote.sortSpeechNotes(aj_globalvar.allSpeechNotesArray)

if __name__ == "__main__":
    soundFile = "./sound/textreadings/1/recording2.wav"
    faceFile = "./sound/textreadingslynnette/1/face.apf"
    #speechProcessor = Speechprocessor(soundFile, faceFile)
    #aj_globalvar.allSpeechNotesArray = aj_speechnote.sortSpeechNotes(aj_globalvar.allSpeechNotesArray)

    #processAllAudioData()
    
    #ACE = aj_speechnote.speechNote()
    #ACE.setNotes(aj_speechnote.Note.A, aj_speechnote.Note.C, aj_speechnote.Note.E)
    #ACE.setAmp(390, 1110, 110)
    #ACE.setFaceData([1,2,3])
    
    #closestSpeechNote = aj_speechnote.findNearestSpeechNote(ACE)
    #print closestSpeechNote.start, closestSpeechNote.peak, closestSpeechNote.end

    soundTesting = "./sound/recording/recording.wav"
    sp = Speechprocessor(soundTesting, faceFile)