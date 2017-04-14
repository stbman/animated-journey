# -*- coding: utf-8 -*-
"""
Created on Mon May 30 21:36:21 2016

@author: Adrian Lim Xuan Wei
"""

import pyaudio
import wave
import struct
import numpy

import aj_graphprocessor
import aj_globalvar
from aj_stopablethread import StopableThread
from aj_plotaudio import PlotAudio

import itertools

CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000

class SoundRecorder(StopableThread):
    def __init__(self, wavPath):
        StopableThread.__init__(self)
        self.stop = False
        self.audio = pyaudio.PyAudio()
        self.wavPath = wavPath
        self.AudioTime = 0
        self.frames = []
        self.readData = 0
        self.count = 0
    '''
        Records audio until key pressed, then saves to file
    '''
    def run(self):

        print "Recording..."
        stream = self.audio.open(format=FORMAT,
                            channels=CHANNELS,
                            rate=RATE,
                            input=True,
                            frames_per_buffer=CHUNK)

        self.frames = []
    
        while True:
            if self.stop:
                break
            data = stream.read(CHUNK)
            self.frames.append(data)
            self.readData += CHUNK

        print "Stopped recording."

        # save the audio data
        wf = wave.open(self.wavPath, 'wb')
        wf.setnchannels(CHANNELS)
        wf.setsampwidth(self.audio.get_sample_size(FORMAT))
        wf.setframerate(RATE)
        wf.writeframes(b''.join(self.frames))

        totalframes = wf.getnframes()
        self.AudioTime = (float)(totalframes) / wf.getframerate()

        wf.close()

        print "Recording saved."

    def stopRecording(self):
        self.stop = True

    def GetAudioTime(self):
        return self.AudioTime

    def IsReady(self):
        return self.readData > RATE

    def GetCurrentFaceData(self):
        #frameString = b''.join(self.frames)
        #frames = 'b' + frameString
        #a = struct.unpack("%ih" % self.readData, frameString)
        #a = [float(val) / pow(2, 15) for val in a]

        #data = stream.read(CHUNKSIZE)
        #frames.append(numpy.fromstring(data, dtype=numpy.int16))

        #outputAudio = b''.join(self.frames)

        oneSec = (RATE / CHUNK)
        countToCut = oneSec / aj_globalvar.audioNormTimeWindow
        castToIntCutter = int(countToCut)

        testcopy = self.frames[:]
        lenTestCopy = len(testcopy)

        segmented = self.frames[-castToIntCutter:]
        lenSegmented = len(segmented);
        
        convertedSegmented = map(lambda x: numpy.fromstring(x, dtype=numpy.int16), segmented)
        lenConvertedSegemented = len(convertedSegmented)

        audioData = list(itertools.chain.from_iterable(convertedSegmented))
        
        #testreadData = len(segmented[0])
        #audioData = numpy.hstack(segmented)
        readData = len(audioData)

        #numpyFrames = map(lambda x: numpy.fromstring(x, dtype=numpy.int16), self.frames)
        #numpy1dFrames = numpy.hstack(numpyFrames)

        threshold = self.CalculateThreshold(audioData)

        #self.count = self.count + 1
        #if (self.count % 5) is 0:
        #    PlotAudio(audioData, threshold, self.count)
        #    faceFrameData = aj_graphprocessor.GetFaceFrameFromData(RATE, audioData, readData - 1, threshold, plot=self.count)
        #else:
        #    faceFrameData = aj_graphprocessor.GetFaceFrameFromData(RATE, audioData, readData - 1, threshold, plot=None)
        
        faceFrameData = aj_graphprocessor.GetFaceFrameFromData(RATE, audioData, readData - 1, threshold)
        return faceFrameData

    def CalculateThreshold(self, audioData):
        threshold = 0.0
        absAudioData = numpy.abs(audioData)
        std = numpy.std(absAudioData)
        sum = numpy.sum(absAudioData)
        length = len(absAudioData)
        avg = numpy.mean(absAudioData)
        #print "STD " + str(std)
        #print "AVG " + str(avg)
        threshold = numpy.abs(avg - std)
        #print "threshold " + str(threshold)

        return threshold

if __name__ == "__main__":
    record = SoundRecorder("./sound/recording/recording.wav")