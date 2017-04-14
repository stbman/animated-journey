# -*- coding: utf-8 -*-
"""

@author: Adrian Lim Xuan Wei
"""

from enum import Enum
import numpy
import aj_globalvar

Note = Enum('Note', ['None', 'C', 'Cis', 'D', 'Dis', 'E', 'F', 'G', 'Gis', 'A', 'Ais', 'B', 'Bis', 'Length'])

fundamentalFreq = {Note.C: 16.35, Note.Cis: 17.32, Note.D: 18.25, Note.Dis: 19.45, Note.E: 20.65, Note.F: 21.83, Note.G: 23.12, Note.Gis: 24.50, Note.A: 25.96, Note.Ais: 27.50, Note.B: 29.14, Note.Bis: 30.87}

distanceWeights = {'startAmp': 1, 'peakAmp': 3, 'endAmp': 1, \
                    'startToPeakTime': 1, 'peakToEndTime': 1, \
                    'startFreq': 1, 'peakFreq': 3, 'endFreq': 1}

#distanceWeights = {'startAmp': 0.11473502, 'peakAmp': 0.11593281, 'endAmp': 0.1150989, \
#                    'startFreq': 0.20468253, 'peakFreq': 0.19525574, 'endFreq': 0.2057193}

class speechNote:

    def __init__(self):
        pass

    def setNotes(self, start=Note.None, peak=Note.None, end=Note.None):
        self.start = start
        self.peak  = peak
        self.end   = end
    
    def setFaceData(self, faceData=[]):
        self.faceData = faceData    # Stores [[faceData0], [faceData1]] rearranged vertices

    def setMidFrameIndex(self, index):
        self.midFrameIndex = index

    def setAmp(self, startAmp=0.0, peakAmp=0.0, endAmp=0.0):
        self.startAmp  = abs(startAmp)
        self.peakAmp   = abs(peakAmp)
        self.endAmp    = abs(endAmp)

    # Time in seconds 
    def setTimeDifference(self, startToPeak=0, peakToEnd=0):
        self.startToPeakTime = startToPeak
        self.peakToEndTime = peakToEnd

class speechNoteWithTime():
    def __init__(self):
        pass

    def setFaceData(self, faceData=[]):
        self.faceData = faceData    # Stores [[faceData0], faceData1]] rearranged vertices

    def setStartTime(self, startTime):
        self.startTime = startTime

    def setMidTime(self, midTime):
        self.midTime = midTime

    def setEndTime(self, endTime):
        self.endTime = endTime

    def setMidFrameIndex(self, midFrameIndex):
        self.midFrameIndex = midFrameIndex

'''
Input: frequency: float 
Output: note: enum note
'''
def getNoteFromFreq(freq) :
    modulous = []

    for key, value in fundamentalFreq.iteritems() :
        modFreq = (value + freq) % value
        modulous.append(modFreq)

    modulous = numpy.array(modulous, numpy.float64)

    idx = numpy.argmin(modulous) + 2    # Because enum 1 is None

    #noteName = Note(idx).name
    noteName = Note(idx)

    return noteName

'''
Find nearest speech note to target note 
Database taken from aj_globalvar.allSpeechNotesArray

Input: targetSpeechNote: speechnote
Output: nearest speech note: speech note
'''
def findNearestSpeechNote(targetSpeechNote):
    distArr = []

    for sn in aj_globalvar.allSpeechNotesArray:
        dist = distBetSpeechNotes(targetSpeechNote, sn)
        distArr.append(dist)

    distArr = numpy.array(distArr, numpy.float64)
    minDistIdx = numpy.argmin(distArr)
    
    closestSpeechNote = aj_globalvar.allSpeechNotesArray[minDistIdx]
    
    return closestSpeechNote

'''
Input: 2 speech notes 
Output : manhattan distance: float 
Ratio: 1 start, 3 peak, 1 end
'''
def distBetSpeechNotes(speechNote1, speechNote2):
    startFreqDiff = abs(fundamentalFreq[speechNote1.start] - fundamentalFreq[speechNote2.start])
    peakFreqDiff  = abs(fundamentalFreq[speechNote1.peak]  - fundamentalFreq[speechNote2.peak])
    endFreqDiff   = abs(fundamentalFreq[speechNote1.end]   - fundamentalFreq[speechNote2.end])

    startAmpDiff  = abs(speechNote1.startAmp - speechNote2.startAmp)
    peakAmpDiff   = abs(speechNote1.peakAmp  - speechNote2.peakAmp)
    endAmpDiff    = abs(speechNote1.endAmp   - speechNote2.endAmp)

    startPeakTimeDiff = abs(speechNote1.startToPeakTime - speechNote2.peakToEndTime)
    peakEndTimeDiff   = abs(speechNote1.peakToEndTime - speechNote2.peakToEndTime)

    difference =  startFreqDiff  * distanceWeights['startFreq'] + \
                    peakFreqDiff * distanceWeights['peakFreq']  + \
                    endFreqDiff  * distanceWeights['endFreq']   + \
                    startAmpDiff * distanceWeights['startAmp']  + \
                    peakAmpDiff  * distanceWeights['peakAmp']   + \
                    endAmpDiff   * distanceWeights['endAmp']    + \
                    startPeakTimeDiff * distanceWeights['startToPeakTime'] + \
                    peakEndTimeDiff   * distanceWeights['peakToEndTime']

    return difference

'''
Sort an array of speech notes
1. Sort by peak
2. Sort by start note
'''
def sortSpeechNotes(speechNoteArray):
    speechNoteArrFirstPass = sorted(speechNoteArray, key=lambda note: note.peak.value)
    speechNoteArrSecondPass = sorted(speechNoteArrFirstPass, key=lambda note: note.start.value)

    return speechNoteArrSecondPass

if __name__ == "__main__":
    ACE = speechNote()
    ACE.setNotes(Note.A, Note.C, Note.E)
    ACE.setFaceData([1,2,3])

    ADC = speechNote()
    ADC.setNotes(Note.A, Note.D, Note.C)
    ADC.setFaceData([4,5,6])

    BCE = speechNote()
    BCE.setNotes(Note.B, Note.C, Note.E)
    BCE.setFaceData([5,6,7])

    arr = [BCE, ADC, ACE]
    print arr[0].faceData, arr[1].faceData, arr[2].faceData

    sortedNotes = sortSpeechNotes(arr)
    # Output should be: ACE ADC BCE
    print sortedNotes[0].faceData, sortedNotes[1].faceData, sortedNotes[2].faceData