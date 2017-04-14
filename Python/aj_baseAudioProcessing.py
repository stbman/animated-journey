# -*- coding: utf-8 -*-
"""
Base Audio Data for Quantitative Analysis

@author: Adrian Lim Xuan Wei
"""

from scipy.io import wavfile
import pickle
import numpy
from array import array

import aj_binaryreader
import aj_graphprocessor
import aj_globalvar

class BaseAudioData():
    def __init__(self, soundFile, faceFile):
        self.baseAudioData = []
        self.originalAudioData = []
        self.generatedFaceAmplitude = []
        self.generatedFaceNote = []

        self.soundFile = soundFile
        self.faceFile = faceFile

        self.storedAudioData = "audioDataGraphFile.pkl"

        self.readFiles()

        self.audioWindow = self.getNumAudioWindowIndices(0.05)  # 0.05 sec

    def readFiles(self):
        # Read audio file
        self.framerate, self.audioData = wavfile.read(self.soundFile)
        self.lengthOfData = len(self.audioData)

        # Read original face file
        self.br = aj_binaryreader.BinaryReader(self.faceFile)
        self.numFaces = self.br.getLengthOfData()

        # Read searched face file
        audioDataFile = open(self.storedAudioData, 'rb')
        aj_globalvar.allSpeechNotesArray = pickle.load(audioDataFile)
        audioDataFile.close()

    def generateFaceData(self):
        for i in xrange(self.numFaces):
            self.originalAudioData.append(self.br.getRearrangedVertices(i))

            # Get Audio index
            audioIndex = self.getAudioIdx(i)
            self.generatedFaceAmplitude.append(self.audioData[audioIndex])

            # Cut audio data to index
            startIndex = audioIndex - 3 * self.audioWindow
            if startIndex < 0:
                startIndex = 0
            
            endIndex = audioIndex
            if endIndex > self.lengthOfData:
                endIndex = self.lengthOfData

            print startIndex, endIndex

            audioSegment = self.audioData[startIndex:endIndex]

            # Calculate audioThreshold
            audioThreshold = self.CalculateThreshold(audioSegment)

            # Generate face data
            faceData, notesArray = aj_graphprocessor.GetFaceFrameFromData(self.framerate, audioSegment, len(audioSegment)-1)

            # Append face data to base audio data
            self.baseAudioData.append(faceData)
            self.generatedFaceNote.append(notesArray)

        aj_globalvar.g_originalFaceData = self.originalAudioData
        aj_globalvar.g_generatedFaceData = self.baseAudioData
        aj_globalvar.g_generatedFaceAmplitude = self.generatedFaceAmplitude
        aj_globalvar.g_generatedFaceNote = self.generatedFaceNote

        # Save file out
        # Or can you do calculations here?
        #output_file = open('generatedFace.apf', 'wb')
        #float_array = array('d', self.baseAudioData)
        #float_array.tofile(output_file)
        #output_file.close()

        '''
        # To Read File
        input_file = open('file', 'r')
        float_array = array('d')
        float_array.fromstring(input_file.read())
        '''

    def getFaceRatio(self, audioIdx):
        audioRatio = float(audioIdx)/ float(self.lengthOfData)
        faceRatio = audioRatio * self.numFaces

        return int(faceRatio)

    def getAudioIdx(self, faceIndex):
        faceRatio = float(faceIndex) / self.numFaces
        audioRatio = faceRatio * float(self.lengthOfData)

        return int(audioRatio)

    def CalculateThreshold(self, audioSegment):
        threshold = 0.0
        absAudioData = numpy.abs(audioSegment)
        std = numpy.std(absAudioData)
        sum = numpy.sum(absAudioData)
        length = len(absAudioData)
        avg = numpy.mean(absAudioData)
        #print "STD " + str(std)
        #print "AVG " + str(avg)
        threshold = numpy.abs(avg - std) / 4
        #print "threshold " + str(threshold)

        return threshold

    def getNumAudioWindowIndices(self, audioTimeWindow):
        audioWindow = audioTimeWindow
        numAudioWindowIndices = audioWindow * self.framerate

        return int(numAudioWindowIndices)

if __name__ == "__main__":
    soundFile = './EmotionAdrian/Base/1/recording2.wav'
    faceFile = './EmotionAdrian/Base/1/face_formatted.apf'
    baseAudioData = BaseAudioData(soundFile, faceFile)
    baseAudioData.generateFaceData()