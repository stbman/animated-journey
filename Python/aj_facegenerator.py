# -*- coding: utf-8 -*

"""

For generating faces array taken in from a speech file to be plotted as display on graphic widget

@author: Adrian Lim Xuan Wei
"""

import numpy

import aj_speechprocessor
import aj_speechnote
import aj_globalvar

class FaceGenerator(aj_speechprocessor.Speechprocessor) :

    def __init__(self, soundFile):
        self.soundFile = soundFile
        self.faceData = []

        self.readSoundFile()
        #self.processSpeech()

    def generateAPF(self) :

        apfData = []

        # set currentFrame as neutral pose(first frame of any data? KIV)
        currentFrame = self.speechNoteTimeArray[0].faceData[0]

        # set currentTime = 0
        currentTime = 0

        # det iterationIndex = 0
        iterationIndex = 0

        # loop speechNoteWithTime till last node
            # set currentNode as speechNoteWithTime[currentNode]
            # TODO: generate frame from currentTime to currentNode.startTime by 
            # interpolating currentFrame to currentNode.startFrame
            # TODO: interpolate speechNoteWithTime data to fit mid and end
            # set currentFrame to endFrame
            # set currentTime to endTime

        for i in xrange(len(self.speechNoteTimeArray)):
            currentNode = self.speechNoteTimeArray[i]

            # start frame face data
            startFrame = currentNode.faceData[0]

            # generate frame from currentTime to currentNode.startTime
            generateTime = currentNode.startTime
            while generateTime > aj_globalvar.updaterate :
                generateTime -= aj_globalvar.updaterate
                apfData.append(currentNode.faceData[0])

            # mid face data
            midFrame = currentNode.midFrameIndex

            timeDiff = currentNode.startTime - currentTime

            # end frame                                            m 
            currentFrame = currentNode.faceData[-1]
            currentTime = currentNode.endTime

        # TODO: generate frame from currentTime to end of audioTime by interpoating currentFrame with neutral pose

    def processSpeech(self) :
        self.speechNoteTimeArray = []

        prevEnd = 0

        for i in xrange(len(self.peaks)):
            currPeak = self.peaks[i]

            SpeechNote = aj_speechnote.speechNote()
            
            #start, peak, end = self.performGradientDescent(currPeak)
            start, peak, end = self.findStartEnd(currPeak)

            if start < prevEnd:
                start = prevEnd + 1
            
            prevEnd = end

            SpeechNote = self.constructSpeechNote(start, peak, end)

            closestSpeechNote = aj_speechnote.findNearestSpeechNote(SpeechNote)

            speechNoteFaceData = closestSpeechNote.faceData

            SpeechNoteTime = aj_speechnote.speechNoteWithTime()
            holder = []

            for fd in speechNoteFaceData:
                holder.append(fd[0])

            SpeechNoteTime.setFaceData(holder)
            SpeechNoteTime.setMidFrameIndex(closestSpeechNote.midFrameIndex)

            startTime = self.getTimeStamp(start)
            endTime = self.getTimeStamp(end)

            #print "start Index: " + str(start) + " end Index: " + str(end) + \
            #    " start Time: " + str(startTime) + " end Time: " + str(endTime)

            SpeechNoteTime.setStartTime(startTime)
            SpeechNoteTime.setMidTime(self.getTimeStamp(peak))
            SpeechNoteTime.setEndTime(endTime)

            #self.faceData.append(speechNoteFaceData[0][0])

            # TODO
            # convert to apf file for reading
            # frames need to be equally(time) apart to other frame for interpolation to work

            self.speechNoteTimeArray.append(SpeechNoteTime)

        x = 10
        #return numpy.array(self.speechNoteTimeArray)
        return self.speechNoteTimeArray

        '''
        Return the time given the index 
        duration in sec = number of frames / frame rate
        '''
    def getTimeStamp(self, index):
        time = float (index) / aj_globalvar.framerate
        return time

if __name__ == "__main__":
    #soundFile = "./sound/textreadings/1/recording2.wav"
    #faceFile = "./sound/textreadings/1/faceProcessed.apf"
    #speechProcessor = aj_speechprocessor.Speechprocessor(soundFile, faceFile)
    #fg = FaceGenerator(soundFile)

    soundTesting = "./sound/recording/recording.wav"
    aj_speechprocessor.processAllAudioData()
    fg = FaceGenerator(soundTesting)
    print aj_globalvar.windowWidth, aj_globalvar.framerate
    fg.processSpeech()