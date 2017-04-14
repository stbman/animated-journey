# -*- coding: utf-8 -*-
"""
Created on Mon May 30 21:36:21 2016

@author: Adrian Lim Xuan Wei
"""
# PyQt imports
from PyQt4 import QtGui, QtOpenGL, QtCore
from PyQt4.QtCore import QTimer

# Numpy imports
import numpy as np
import numpy.random as rdn

# Scipy imports 
from scipy.io import wavfile 

# Matplotlib imports
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
import matplotlib.pyplot as plt 

import pickle

# Custom imports
import aj_glplotwidget
import aj_texttospeech
import aj_emotions
import aj_binaryreader
import aj_soundRecord
import aj_globalvar

import aj_speechprocessor
import aj_facegenerator
import aj_graphprocessor
import aj_faceframeevaluator
import aj_baseAudioProcessing

from aj_texttospeech import play_wav_file, getDurationWavFile
from aj_soundRecord import SoundRecorder
from aj_facegenerator import FaceGenerator

subDir = 'Neutral/c'
wavFileName  = aj_globalvar.rootDir + subDir + '/' + aj_globalvar.recordingFile
dataFileName = aj_globalvar.rootDir + subDir + '/' + aj_globalvar.sampleFile
faceDataFileName = aj_globalvar.rootDir + subDir + '/' + aj_globalvar.faceFile
anchorDataFileName = aj_globalvar.rootDir + aj_globalvar.anchorFile
recordwavFileName = './sound/recording/recording.wav'
testAdrianWavFileName = './sound/recording/adrianTest.wav'
testWavFileName = './sound/testdata/hellopenguin.wav'
interpolationFile = aj_globalvar.rootDir + aj_globalvar.interpolationFile

import aj_globalvar
aj_globalvar.audioThread = 0

class MainWindow(QtGui.QMainWindow):

    def __init__(self):
        super(MainWindow, self).__init__()
        
        # Initialise other classes
        self.emotionAnalyser = aj_emotions.EmotionAnalyzer()

        # Read face interpolation file
        aj_globalvar.g_InterpolationData = np.loadtxt(interpolationFile, dtype=float, delimiter=",")

        DataSearch = aj_globalvar.g_InterpolationData.copy()
        DataSearch.sort()

        g_InterpolationDataSize = len(aj_globalvar.g_InterpolationData)
        cullIndexForData = int(g_InterpolationDataSize * 0.2)
        cullValue = aj_globalvar.g_InterpolationData[cullIndexForData]

        for x in xrange(len(aj_globalvar.g_InterpolationData)):
            if aj_globalvar.g_InterpolationData[x] > cullValue:
                aj_globalvar.g_InterpolationIndex.append(x)

        g_InterpolationIndexSize = len(aj_globalvar.g_InterpolationIndex)

        # For quantitative evaluation 
        if (aj_globalvar.g_RenderMode == 1 or aj_globalvar.g_RenderMode == 2):
            print "Generating Quantitative Evaluation Data"
            self.baseAudioData = aj_baseAudioProcessing.BaseAudioData(wavFileName, faceDataFileName)
            self.baseAudioData.generateFaceData()

        # Read audio database
        #aj_speechprocessor.processAllAudioData()

        # Testing read from file 
        audioDataFile = open(aj_globalvar.allSpeechNotesFile, 'rb')
        aj_globalvar.allSpeechNotesArray = pickle.load(audioDataFile)
        audioDataFile.close()

        self.wavFileCounter = 0
        self.graphProcessor = aj_graphprocessor.GraphProcessor()

        anchorFaceReader = aj_binaryreader.BinaryReader(anchorDataFileName)
        aj_globalvar.g_AnchorFace = anchorFaceReader.rearrangeVertices().copy()

        # Face data
        aj_globalvar.g_BinaryReader = aj_binaryreader.BinaryReader(dataFileName)
        aj_globalvar.g_RenderData = aj_globalvar.g_BinaryReader.ConvertAPFtoSpeechNoteTime()

        self.frame_count = 0
        self.get_data()

        rate, self.soundData = wavfile.read(wavFileName)

        wavFileDuration = aj_texttospeech.getDurationWavFile(wavFileName)
        self.timeout = 0# np.ceil(float(wavFileDuration) / self.totalFrames)
        
        self.plotFrame = 0
        #self.plotTimer = QTimer()
        #self.plotTimer.timeout.connect(self.plotTimerTick)
        #self.plotTimer.start(1000)
                
        self.textBox = QtGui.QLineEdit() 
        self.textBox.textChanged.connect(self.textChanged)
        
        self.animateButton = QtGui.QPushButton('', self)
        #self.animateButton.clicked.connect(self.animateClicked)
        self.animateButton.setEnabled(False)
        
        self.recordButton = QtGui.QPushButton('Start Recording Audio', self)
        self.recordButton.clicked.connect(self.recordClicked)
        self.isRecording = False
        
        self.indicatorBox = QtGui.QHBoxLayout()
        self.textLabel = QtGui.QLabel()
        #self.textLabel.setText("Text")
        self.indicatorLabel = QtGui.QLabel()
        
        self.updateFrameLabel()
        self.indicatorBox.addWidget(self.textLabel)
        self.indicatorBox.addWidget(self.indicatorLabel)
        
        self.TriggerBenchmark = False;

        # Keyboard shortcut to replay
        self.connect(QtGui.QShortcut(QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_R), self),
                     QtCore.SIGNAL('activated()'), self.replay)

        self.connect(QtGui.QShortcut(QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_T), self),
                     QtCore.SIGNAL('activated()'), self.triggerTest)

        self.connect(QtGui.QShortcut(QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_P), self),
                     QtCore.SIGNAL('activated()'), self.triggerPenguin)
        
        self.connect(QtGui.QShortcut(QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_M), self),
                     QtCore.SIGNAL('activated()'), self.useMesh)

        self.connect(QtGui.QShortcut(QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_B), self),
                     QtCore.SIGNAL('activated()'), self.TriggerQuantitativeBenchmark)
        self.connect(QtGui.QShortcut(QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_0), self),
                     QtCore.SIGNAL('activated()'), self.TriggerRenderMode0)
        self.connect(QtGui.QShortcut(QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_1), self),
                     QtCore.SIGNAL('activated()'), self.TriggerRenderMode1)
        self.connect(QtGui.QShortcut(QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_2), self),
                     QtCore.SIGNAL('activated()'), self.TriggerRenderMode2)

        # Set GL Plot Widget
        glformat = QtOpenGL.QGLFormat()
        glformat.setVersion(3, 3)
        glformat.setProfile(QtOpenGL.QGLFormat.CoreProfile)
        self.widget = aj_glplotwidget.OpenGlWidget(glformat)
        #self.widget.set_data(self.data)
        
        # Matplotlib widget
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(111)
        self.ax.plot(self.soundData, color='blue')

        self.canvas = self.ax.figure.canvas

        self.fig.canvas.draw()
        self.background = self.canvas.copy_from_bbox(self.ax.bbox)
        self.drawline = self.ax.axvline(x=0, linewidth=2, color='r')

        #plt.show(block=False)
        #self.drawline = self.ax.axvline(x=0, linewidth=10, color='r')
        #plt.plot(self.soundData, color='blue')
        #self.toolbar = NavigationToolbar(self.canvas, self)
        #self.plot()

        mainLayout = QtGui.QVBoxLayout()
        #mainLayout.addWidget(self.textBox)
        mainLayout.addWidget(self.animateButton)
        mainLayout.addWidget(self.recordButton)
        mainLayout.addLayout(self.indicatorBox)
        mainLayout.addWidget(self.widget)
        mainLayout.addWidget(self.canvas)
        mainLayout.setSpacing(0)
        
        mainWidget = QtGui.QWidget()
        mainWidget.setLayout(mainLayout)
        self.setCentralWidget(mainWidget)
        
        self.setWindowTitle("Adrian Thesis")

        self.resize(600,600)

        self.show()

        # Set Timer
        self.timer = QTimer()
        self.timer.timeout.connect(self.timerTick)
        self.timer.start(self.timeout)
    
    def textChanged(self):
        text = self.textBox.text()
        if text == "":
            self.animateButton.setEnabled(False)
        else:
            self.animateButton.setEnabled(True)

    def animateClicked(self):
        text = self.textBox.text()
        self.textBox.setText(text)
        self.textLabel.setText("Text: " + text)
        
        emotion_scores = self.emotionAnalyser.analyzeEmotion(text)
        print emotion_scores
        
        self.textBox.setText("")

        # Construct a wav file
        wavFileName = str(wavFileCounter) + '.wav'
        wavFileCounter += 1
        aj_texttospeech.processString(text, wavFileName)

        # Get Face Data 
        faceData = aj_facegenerator(wavFileName)

        # TODO: Should output faceData as animation
        
    def recordClicked(self):
        self.isRecording = not self.isRecording
        if (self.isRecording):
            aj_globalvar.audioThread.stop()
            while aj_globalvar.audioThread.IsRunning():
                pass

            aj_globalvar.g_SoundRecorder = SoundRecorder(recordwavFileName)
            aj_globalvar.g_SoundRecorder.start()
            self.recordButton.setText("Stop Recording Audio")
            
        else:
            aj_globalvar.g_SoundRecorder.stopRecording()
            self.recordButton.setText("Start Recording Audio")

            #speechData = self.graphProcessor.getClosestMatch(testAdrianWavFileName)
            #faceData = [sd['faceData'] for sd in speechData]
            #aj_globalvar.g_RenderData = faceData

            #faceGenerator = FaceGenerator(recordwavFileName)
            #aj_globalvar.g_RenderData = faceGenerator.processSpeech()
            aj_globalvar.audioThread = PlayAudio(recordwavFileName)
            aj_globalvar.audioThread.start()

            #theData = aj_globalvar.g_RenderData
            #print "Total Nodes: " + str(len(theData))
            #print "Audio Length: " + str(aj_globalvar.audioThread.GetAudioTime())
            #for i in xrange(len(theData)):
            #    print "Start Time: " + str(theData[i].startTime) + " End Time: " + str(theData[i].endTime) + " Total Frames:" + str(len(theData[i].faceData))
            #faceGenerator = FaceGenerator(testWavFileName)
            #faceData = faceGenerator.processSpeech()
            #compare = aj_globalvar.g_RenderData
            #aj_globalvar.g_RenderData = faceGenerator.processSpeech()
            #
            #aj_globalvar.audioThread = PlayAudio(testWavFileName)
            #aj_globalvar.audioThread.start()
        
    def triggerTest(self):
        aj_globalvar.audioThread.stop()
        while aj_globalvar.audioThread.IsRunning():
            pass

        #speechData = self.graphProcessor.getClosestMatch(testAdrianWavFileName)
        #faceData = [sd['faceData'] for sd in speechData]
        #aj_globalvar.g_RenderData = faceData

        #faceGenerator = FaceGenerator(testAdrianWavFileName)
        #aj_globalvar.g_RenderData = faceGenerator.processSpeech()
        aj_globalvar.audioThread = PlayAudio(testAdrianWavFileName)
        aj_globalvar.audioThread.start()

        #theData = aj_globalvar.g_RenderData
        #print "Total Nodes: " + str(len(theData))
        #print "Audio Length: " + str(aj_globalvar.audioThread.GetAudioTime())
        #for i in xrange(len(theData)):
        #    print "Start Time: " + str(theData[i].startTime) + " End Time: " + str(theData[i].endTime) + " Total Frames:" + str(len(theData[i].faceData))
            

    def TriggerRenderMode0(self):
        aj_globalvar.g_RenderMode = 0;
    def TriggerRenderMode1(self):
        aj_globalvar.g_RenderMode = 1;
    def TriggerRenderMode2(self):
        aj_globalvar.g_RenderMode = 2;

    def TriggerQuantitativeBenchmark(self):
        if self.TriggerBenchmark:
            self.TriggerBenchmark = not self.TriggerBenchmark;
            self.TriggerBenchmarkEndtime = aj_globalvar.audioThread.GetCompletionRatio();

            # run benchmark from start to end time
            # loop all the source frame and compare
            # print out score

            # fetch face frames from source
            base = aj_globalvar.g_originalFaceData

            # generate face frames from data base
            target = aj_globalvar.g_generatedFaceData
            
            # they must both have the same amount of face frame

            frameCount = len(aj_globalvar.g_generatedFaceData)
            startFrame = int(self.TriggerBenchmarkStarttime * frameCount)
            endFrame = int(self.TriggerBenchmarkEndtime * frameCount)

            # compare face frames and give result
            score = aj_faceframeevaluator.EvaluateFaceFrame(startFrame, endFrame);

            print "Benchmark from " + str(startFrame) + " to " + str(endFrame) + " has a total of " + str(endFrame - startFrame) + " frames."

        else:
            self.TriggerBenchmark = not self.TriggerBenchmark;
            self.TriggerBenchmarkStarttime = aj_globalvar.audioThread.GetCompletionRatio();


    def triggerPenguin(self):
        aj_globalvar.audioThread.stop()
        while aj_globalvar.audioThread.IsRunning():
            pass

        faceGenerator = FaceGenerator(testWavFileName)
        aj_globalvar.g_RenderData = faceGenerator.processSpeech()
        aj_globalvar.audioThread = PlayAudio(testWavFileName)
        aj_globalvar.audioThread.start()

        theData = aj_globalvar.g_RenderData
        print "Total Nodes: " + str(len(theData))
        print "Audio Length: " + str(aj_globalvar.audioThread.GetAudioTime())
        for i in xrange(len(theData)):
            print "Start Time: " + str(theData[i].startTime) + " End Time: " + str(theData[i].endTime) + " Total Frames:" + str(len(theData[i].faceData))
            
    def useMesh(self):
        aj_globalvar.useMesh = not aj_globalvar.useMesh

    def get_data(self):
        self.data, self.totalFrames = aj_globalvar.g_BinaryReader.getRearrangedVertices(self.frame_count)

    def timerTick(self):
        self.widget.updateGL()
        self.frame_count = int(aj_globalvar.audioThread.GetCompletionRatio() * self.totalFrames) - 1
        self.updateFrameLabel()
    
        #self.widget.set_data(self.data)

        self.plot()

    def plotTimerTick(self):
        if self.plotFrame < len(self.soundData) -1:
            self.plotFrame += 16000

    def updateFrameLabel(self):
        if (aj_globalvar.audioThread != 0):
            currentRatio = aj_globalvar.audioThread.GetCompletionRatio()
            totalTime = aj_globalvar.audioThread.GetAudioTime()
            self.indicatorLabel.setText('Time: ' + str(currentRatio * totalTime) +
                            ' / ' + str(totalTime))
        else:
            self.indicatorLabel.setText('No Data')
                        
    def replay(self):
        #aj_globalvar.audioThread

        print "Replay"
        aj_globalvar.audioThread.RestartPlayer()
        #while aj_globalvar.audioThread.IsRunning():
        #    pass
        self.frame_count = 0
        self.plotFrame = 0
        #aj_globalvar.audioThread =  PlayAudio(wavFileName) #should pass in the file name
        #aj_globalvar.audioThread.start()

    def plot(self):
        #ax = self.figure.add_subplot(111)
        #self.ax.hold(False)

        #ax.plot(self.soundData, color='blue')
        #ax.axvline(x=self.plotFrame, linewidth=2, color='r')
        self.canvas.restore_region(self.background)
        if aj_globalvar.audioThread is not 0:
            currentFrame = int(aj_globalvar.audioThread.GetCompletionRatio() * len(self.soundData))
            
            self.drawline.set_xdata(currentFrame)
            self.ax.draw_artist(self.drawline)
            self.canvas.blit(self.ax.bbox)
        #self.fig.canvas.draw()

from aj_audioplayer import PlayAudio

if __name__ == '__main__':
    app = QtGui.QApplication(['Adrian Thesis'])
    window = MainWindow()
    window.show()

    aj_globalvar.audioThread = PlayAudio(wavFileName)
    aj_globalvar.audioThread.start()

    app.exec_()