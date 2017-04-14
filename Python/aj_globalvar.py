# -*- coding: utf-8 -*-
"""

@author: Adrian Lim Xuan Wei
"""

audioThread     = 0
updaterate      = 0.05       # 50 ms in sec
framerate       = 16000      # Currently Kinect frame rate
windowWidth     = updaterate*3*2

# Directories names
#rootDir = "./sound/textreadingsadrian/"
rootDir = "./EmotionAdrian/"
recordingFile = "recording2.wav"    # rootDir + subDir + recordingFile
faceFile = "face_formatted.apf"
sampleFile = "face.apf"
anchorFile = "anchor.apf"
interpolationFile = "interpolationMap.csv"

import fundamentalFreq
fundamentalFreqDict = fundamentalFreq.fundamentalFreq   # fundamental freq dictionary (list of dicts {'freq': float, 'note': string, 'octave': int} )

freqToFaceDict = {}

allSpeechNotesArray = []
audioWindow = 0.10  # sec
allSpeechNotesFile = "audioDataGraphFile.pkl"

g_BinaryReader = 0

g_RenderData = []

useMesh = True

g_SoundRecorder = 0

g_AnchorFace = []

g_InterpolationData = []
g_InterpolationIndex = []

g_RenderMode = 1

# For audio normalisation
audioNormTimeWindow = 1.0
audioNormNumFrames = int (framerate * audioNormTimeWindow)

# For quantitative evaluation
g_originalFaceData = []
g_generatedFaceData = []
g_generatedFaceAmplitude = []
g_generatedFaceNote = []