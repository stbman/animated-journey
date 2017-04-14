
# -*- coding: utf-8 -*-
"""
@author: Adrian Lim Xuan Wei
"""

import scipy
from scipy.fftpack import fft
from scipy.io import wavfile
import scipy.signal
from python_speech_features import mfcc, logfbank

import matplotlib.pyplot as plt

import numpy 

from aj_detectpeaks import detect_peaks
import aj_speechnote
import aj_binaryreader
import aj_globalvar

MAX_WAV16_AMP = 32767.0  # = 2**15-1  # because wav16 is signed

class Speechprocessor():
    
    def __init__(self, soundFile, faceFile, addToGlobal=False):
        self.soundFile = soundFile
        self.faceFile = faceFile
        self.addToGlobal = addToGlobal

        self.intervalBetweenPeak = aj_globalvar.audioWindow # secs
        self.intervalBeforePeak = aj_globalvar.audioWindow

        self.speechData = []

        self.readSoundFile()
        self.audioData = self.normaliseAudio()

        self.plotCount = 0

        if self.faceFile is not None:
            self.initializeBinaryReader()
        self.processSpeech()

    def readSoundFile(self):
        aj_globalvar.framerate , self.audioData = wavfile.read(self.soundFile)
        self.audioData = self.audioData / MAX_WAV16_AMP
        self.maxAudioData = max(self.audioData)
        self.minAudioData = min(self.audioData)
        #self.audioData = self.audioData / aj_globalvar.framerate

        self.audioWindowIndices = self.getAudioWindowIndices(self.intervalBetweenPeak)
        self.audioBeforePeakWindowIndices = 5 * self.audioWindowIndices

        self.numWindowIndices = self.getNumWindowIndices()
        self.kinectWindowIndices = self.getKinectWindowIndices()

        windowSize = 256
        self.halfWindow = int(windowSize/2)
        self.lengthOfData = len(self.audioData)

    def normaliseAudio(self):
        rmsArray = []
        stepInterval = aj_globalvar.audioNormNumFrames

        start = stepInterval
        end = start + stepInterval

        endOfLoop = len(self.audioData - stepInterval)

        # find rms of time interval 2.0s 
        for i in xrange(stepInterval, endOfLoop, stepInterval):
            intervalArray = self.audioData[start : end]
            intervalRms = self.rootMeanSquare(intervalArray)
            rmsArray.append(intervalRms)

        # find max rms 
        maxRms = max(rmsArray)

        # divide audio by max rms
        #dividedAudio = [x / maxRms for x in self.audioData]
        dividedAudio = self.audioData / maxRms

        # clamp audio signal
        clampedAudio = numpy.clip(dividedAudio, self.minAudioData, self.maxAudioData)

        return clampedAudio

    def initializeBinaryReader(self):
        self.br = aj_binaryreader.BinaryReader(self.faceFile)
        self.numFaces = self.br.getLengthOfData()

    def processSpeech(self):
        for i in xrange(self.audioBeforePeakWindowIndices, self.lengthOfData-self.audioBeforePeakWindowIndices, self.audioWindowIndices):
            speechData = {'notes': [], 'audio': [], 'faceData': []}

            #if i < self.audioBeforePeakWindowIndices:
            #    start = 0
            #else:
            #    start = self.audioBeforePeakWindowIndices

            start = i - self.audioBeforePeakWindowIndices

            for j in xrange(start, i, self.audioWindowIndices):
                amplitude = self.audioData[j]
                fundamentalFreq = self.findFundamentalFreqOfPoint(j)
                note = self.findNoteOfOnePeak(fundamentalFreq)

                speechData['notes'].append(note.name)
                speechData['audio'].append(amplitude)
                
            # Get the mfcc features 
            truncatedAudio = self.audioData[start:i]
            mfccFeatures = mfcc(truncatedAudio)[0]
            speechData['mfcc'] = mfccFeatures

            # Get the rearranged face data at the peak
            faceRatio = self.getFaceRatio(i)
            faceData = self.br.getRearrangedVertices(faceRatio)
            speechData['faceData'] = faceData

            self.speechData.append(speechData)

            x = 10

            if self.addToGlobal:
                aj_globalvar.allSpeechNotesArray.append(speechData)


    def getAudioWindowIndices(self, interval):
        indices = float (aj_globalvar.framerate) * interval
        return int (indices)

    def getKinectWindowIndices(self):
        indices = float (self.intervalBetweenPeak) / aj_globalvar.framerate
        return int (indices)

    def getNumWindowIndices(self):
        indices = float (aj_globalvar.windowWidth) * aj_globalvar.framerate
        # Floor because indices is a number
        return int (indices)

    '''
    Takes in 1D Array and outputs Root Mean Square of Array
    '''
    def rootMeanSquare(self, array):
        return numpy.sqrt(numpy.mean(numpy.square(array)))

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
        stepInterval = aj_globalvar.audioNormNumFrames
        endOfLoop = self.lengthOfData - stepInterval

        left = point - self.halfWindow
        if left < 0:
            left = 0
        right = point + self.halfWindow
        if right > self.lengthOfData:
            right = self.lengthOfData

        truncatedData = self.audioData[left:right]
        autoCorrData = self.autocorrelation(truncatedData)

        #fftData = scipy.fftpack.fft(autoCorrData)
        fftData = self.performSTFT(autoCorrData)
        halfFFTData = numpy.abs(fftData[:len(autoCorrData)/2+1])    # FFT is symmetrical
        fundamentalFreq = numpy.argmax(halfFFTData)

        return fundamentalFreq

    def findNotesOfPeaks(self, leftFreq, peakFreq, rightFreq):
        leftNote  = self.findNoteOfOnePeak(leftFreq)
        peakNote  = self.findNoteOfOnePeak(peakFreq)
        rightNote = self.findNoteOfOnePeak(rightFreq)

        return leftNote, peakNote, rightNote

    '''
    Short Time Fourier Transform
    '''
    def performSTFT(self, data):
        dataLength = len(data)

        zeroPad = numpy.zeros(dataLength)
        window = scipy.signal.kaiser(dataLength, 14)
        windowedSignal = data * window 
        processedSignal = scipy.fftpack.fft(windowedSignal)

        '''
        plt.clf()

        plt.figure(1)
        plt.subplot(311)
        plt.plot(data)
        plt.subplot(312)
        plt.plot(windowedSignal)
        plt.subplot(313)
        plt.plot(processedSignal)
        plt.savefig('./plots/stft/' + str(self.plotCount))
        self.plotCount = self.plotCount + 1

        # Plot spectrogram
        plt.clf()
        samplingFreq = 0.05
        Fs = int(1/0.05)
        plt.specgram(processedSignal, NFFT=256, Fs=Fs)
        plt.savefig('./plots/stft/' + str(self.plotCount))
        self.plotCount = self.plotCount + 1
        '''

        return processedSignal

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
        soundFile = ''
        faceFile = ''
        for f in files:
            if f.endswith(aj_globalvar.recordingFile):
                soundFile = os.path.join(subdir, f)
            if f.endswith(aj_globalvar.faceFile):
                faceFile = os.path.join(subdir, f)
            
        #print soundFile, faceFile
        if soundFile != '' and faceFile != '':
            soundFile = soundFile.replace('\\', '/')
            faceFile = faceFile.replace('\\', '/')
            sp = Speechprocessor(soundFile, faceFile, True)

    # Save to file
    import pickle 
    audioDataFile = open(aj_globalvar.allSpeechNotesFile, 'wb')
    pickle.dump(aj_globalvar.allSpeechNotesArray, audioDataFile)
    audioDataFile.close()

def normaliseAudioWriteToFile(inAudioFile, outAudioFile):
    faceFile = "./Emotion/Anger/1/face.apf"


def plotSpectrogram(soundFile):
    framerate , audioData = wavfile.read(soundFile)

    dataLength = len(audioData)

    zeroPad = numpy.zeros(dataLength)
    window = scipy.signal.kaiser(dataLength, 14)
    windowedSignal = audioData * window 
    processedSignal = scipy.fftpack.fft(windowedSignal)
    processedSignal = processedSignal[:len(processedSignal)/2+1]

    # Plot spectrogram
    plt.clf()
    samplingFreq = 0.05
    Fs = int(1/0.05)
    plt.specgram(processedSignal, NFFT=256, Fs=Fs, noverlap=5)
    plt.savefig('./plots/stft/kaiser')


if __name__ == "__main__":
    soundFile = "./Emotion/Anger/b/recording2.wav"
    faceFile = "./Emotion/Anger/1/face.apf"
    #speechProcessor = Speechprocessor(soundFile, faceFile)
    #aj_globalvar.allSpeechNotesArray = aj_speechnote.sortSpeechNotes(aj_globalvar.allSpeechNotesArray)

    #processAllAudioData()
    
    #ACE = aj_speechnote.speechNote()
    #ACE.setNotes(aj_speechnote.Note.A, aj_speechnote.Note.C, aj_speechnote.Note.E)
    #ACE.setAmp(390, 1110, 110)
    #ACE.setFaceData([1,2,3])
    
    #closestSpeechNote = aj_speechnote.findNearestSpeechNote(ACE)
    #print closestSpeechNote.start, closestSpeechNote.peak, closestSpeechNote.end

    soundTesting = './sound/recording/adrianTest.wav'
    sp = Speechprocessor(soundTesting, faceFile)
    #normalisedAudio = sp.normaliseAudio()
    #wavfile.write('./sound/recording/adrianTestNormalised.wav', aj_globalvar.framerate, normalisedAudio)
    #plotSpectrogram(soundTesting)