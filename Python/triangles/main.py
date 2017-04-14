# -*- coding: utf-8 -*-
"""


Main.py Triangles

Created on Mon May 30 21:36:21 2016
@author: Adrian Lim Xuan Wei
"""
# PyQt imports
from PyQt4 import QtGui, QtOpenGL, QtCore
from PyQt4.QtCore import QTimer

# Numpy imports
import numpy as np

# Custom imports
import glplotwidget
import readBinary

import sys
sys.path.append('..')

from aj_texttospeech import play_wav_file, getDurationWavFile

wavFileName = '../sound/textreadings/b/recording2.wav'

class MainWindow(QtGui.QWidget):
    
    def __init__(self):
        super(MainWindow, self).__init__()
        
        # Globals
        #self.timeout = 50
        self.dataFile = '../sound/textreadings/b/face.apf'
        

        self.binary_data = readBinary.readBinaryFile(self.dataFile)
        self.frame_count = 0
        
        wavFileDuration = getDurationWavFile(wavFileName)
        self.timeout =  np.ceil(float(wavFileDuration) / len(self.binary_data))
        print wavFileDuration, len(self.binary_data), self.timeout
        
        # Set Timer
        self.timer = QTimer()
        self.timer.timeout.connect(self.timerTick)
        self.timer.start(self.timeout)
        
        # Get Data 
        self.indices = readBinary.readIndicesFile()
        
        self.get_data()
        
        self.textBox = QtGui.QLineEdit() 
        self.textBox.textChanged.connect(self.textChanged)
        
        self.animateButton = QtGui.QPushButton('Animate', self)
        self.animateButton.clicked.connect(self.animateClicked)
        self.animateButton.setEnabled(False)
        
        self.widget = glplotwidget.GLPlotWidget(self)
        self.widget.set_data(self.data)
        
        # Keyboard shortcut to replay
        self.connect(QtGui.QShortcut(QtGui.QKeySequence(QtCore.Qt.CTRL + QtCore.Qt.Key_R), self), 
                     QtCore.SIGNAL('activated()'), self.replay)
                     
        mainLayout = QtGui.QVBoxLayout()
        mainLayout.addWidget(self.textBox)
        mainLayout.addWidget(self.animateButton)
        mainLayout.addWidget(self.widget)
        self.setLayout(mainLayout)
        
    def textChanged(self):
        text = self.textBox.text()
        if text == "":
            self.animateButton.setEnabled(False)
        else:
            self.animateButton.setEnabled(True)
        
    def animateClicked(self):
        pass
        
    def get_data(self): 
        self.curr_data = self.binary_data[self.frame_count]
        temp_data = self.curr_data[['posx', 'posy', 'posz']].copy()
        self.data = temp_data[self.indices]
        
    def timerTick(self):
        if self.frame_count < len(self.binary_data)-1:
            self.frame_count += 1
            self.get_data()
            
            self.widget.set_data(self.data)
            self.widget.updateGL()

    def replay(self):
        self.frame_count = 0
        # TODO: REPLAY THE SOUND TOO
        thr.run()
        thr2.run()

if __name__ == '__main__':
    import threading

    app = QtGui.QApplication(['Adrian Thesis'])
    window = MainWindow()
    window.show()
    #app.exec_()
    #play_wav_file('./resources/1/recording2.wav')
        
    thr = threading.Thread(target=play_wav_file, kwargs={'wavFileName': wavFileName})
    thr.start()
    
    thr2 = threading.Thread(target=app.exec_(), args=(), kwargs={})
    thr2.start() 