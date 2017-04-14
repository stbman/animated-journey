from aj_stopablethread import StopableThread
from scipy.io import wavfile
import numpy

from aj_plotaudio import PlotAudio

class PlayAudio(StopableThread):
    wavFileName = 0
    currentCompletionRatio = 0.0

    def __init__(self, filepath):
        StopableThread.__init__(self)
        self.wavFileName = filepath
        self.audioTime = 0.0;
        self.ShouldRestart = False;

        self.framerate , self.audioData = wavfile.read(self.wavFileName)

        self.threshold = self.CalculateThreshold()

        #PlotAudio(self.audioData, self.threshold)

    def CalculateThreshold(self):
        threshold = 0.0
        absAudioData = numpy.abs(self.audioData)
        std = numpy.std(absAudioData)
        sum = numpy.sum(absAudioData)
        length = len(absAudioData)
        avg = numpy.mean(absAudioData)
        #print "STD " + str(std)
        #print "AVG " + str(avg)
        threshold = numpy.abs(avg - std) / 4
        #print "threshold " + str(threshold)

        return threshold

    def run(self):
        import pyaudio
        import wave
        import subprocess 
        import os.path

        wf = wave.open(self.wavFileName, 'rb')
        pa = pyaudio.PyAudio()

        #print "audio framerate: " + str(wf.getframerate())
        #print wf.getframerate()

        # open stream based on the wave object which has been input.
        stream = pa.open(format =
                        pa.get_format_from_width(wf.getsampwidth()),
                        channels = wf.getnchannels(),
                        rate = wf.getframerate(),
                        output = True)
        # read data 
        chunk = 512
        data = wf.readframes(chunk)
        currentFrame = 0
        totalframes = wf.getnframes()
        self.audioTime = (float)(totalframes) / wf.getframerate()

        while not self.ToStop():
            # play stream (looping from beginning of file to the end)
            while data != '':
                if self.ShouldRestart:
                    currentFrame = 0
                    wf = wave.open(self.wavFileName, 'rb')
                    self.ShouldRestart = False
                # writing to the stream is what *actually* plays the sound.
                stream.write(data)
                data = wf.readframes(chunk)
                currentFrame += chunk
                self.currentCompletionRatio = max(0.0, min(1.0, float(currentFrame) / float(totalframes)))
            if self.ShouldRestart:
                currentFrame = 0
                wf = wave.open(self.wavFileName, 'rb')
                self.ShouldRestart = False
                data = wf.readframes(chunk)

        # cleanup stuff.
        stream.close()
        pa.terminate()

        self.running = False;

    def GetCompletionRatio(self):
        return self.currentCompletionRatio

    def GetAudioTime(self):
        return self.audioTime

    def RestartPlayer(self):
        self.ShouldRestart = True
    