# -*- coding: utf-8 -*-
"""
Created on Sun May 29 17:05:11 2016

@author: Adrian Lim Xuan Wei
"""

import numpy as np
import aj_speechnote
import aj_globalvar

class BinaryReader():
    def __init__(self, fileName):
        #fileName = './sound/textreadings/b/face.apf'
        self.fileName = fileName
        self.data = self.readBinaryFile(fileName)
        self.frameCount = len(self.data)
        
        self.indices = self.readIndicesFile()
        
    def readBinaryFile(self, fileName):
        frameChunk = 1347
        
        f = open(fileName, 'rb')
            
        dtype = np.dtype([
            ("posx", np.float32),
            ("posy", np.float32),
            ("posz", np.float32),
            ("texu", np.float32),
            ("texv", np.float32),
            ("norx", np.float32),
            ("nory", np.float32),
            ("norz", np.float32)
        ])
        
        data = np.fromfile(f, dtype=dtype)
    
        # TODO: SCALE PROPERLY    
        data['posx'] = data['posx'] * 3
        data['posy'] = data['posy'] * 3
        data['posz'] = data['posz']/100
        
        numFrames = len(data) / frameChunk
        dataSplit = np.split(data, numFrames)
    
        return dataSplit
    
    def readIndicesFile(self):
        f = open('./resources/indices.apf', 'rb')
        
        data = np.fromfile(f, dtype=np.uint32)
        
        return data
        
    def getRearrangedVertices(self, index):
        d = self.data[index]
        dCut = d[['posx', 'posy', 'posz', 'texu', 'texv']]
        
        return dCut, self.frameCount

    def getRearrangedVerticesForPairs(self, first, second, ratio):
        firstD = self.data[first]
        secondD = self.data[second]
        firstCut = firstD[['posx', 'posy', 'posz', 'texu', 'texv']]
        secondCut = secondD[['posx', 'posy', 'posz', 'texu', 'texv']]
        invertRatio = 1.0 - ratio

        fc = firstCut.copy()

        for i in xrange(len(firstCut)):
            fc[i][0] = (firstCut[i][0] * ratio) + (secondCut[i][0] * invertRatio)
            fc[i][1] = (firstCut[i][1] * ratio) + (secondCut[i][1] * invertRatio)
            fc[i][2] = (firstCut[i][2] * ratio) + (secondCut[i][2] * invertRatio)

        return fc, self.frameCount
        
    def rearrangeVertices(self):
        data = self.readBinaryFile(self.fileName)
        
        indices = self.readIndicesFile()
        
        d = data[0]
        dCut = d[['posx', 'posy', 'posz', 'texu', 'texv']]
        
        return dCut

    def getLengthOfData(self):
        return len(self.data)

    def GetFrameCount(self):
        return self.frameCount

    def ConvertAPFtoSpeechNoteTime(self):
        SpeechNoteTime = aj_speechnote.speechNoteWithTime()

        dataArray = []
        for i in xrange(len(self.data)):
            currentFrame = self.data[i]
            currentFrameFormatted = currentFrame[['posx', 'posy', 'posz', 'texu', 'texv']]
            #rearrangedFrame = currentFrameFormatted[self.indices]
            dataArray.append(currentFrameFormatted)

        SpeechNoteTime.setFaceData(dataArray)
        SpeechNoteTime.setMidFrameIndex(self.frameCount / 2)
        
        SpeechNoteTime.setStartTime(0)
        SpeechNoteTime.setMidTime((self.frameCount / 2) * aj_globalvar.updaterate)
        SpeechNoteTime.setEndTime(self.frameCount * aj_globalvar.updaterate)

        resultArray = []
        resultArray.append(SpeechNoteTime)
        return resultArray

if __name__ == "__main__":
    import pandas
    import numpy 

    binaryReader = BinaryReader('./sound/textreadings/b/faceProcessed.apf')
    thisFrame, _ = binaryReader.getRearrangedVertices(0)
    nextFrame, _ = binaryReader.getRearrangedVertices(1)

    t = 0.5
    y = 1 - t

    zippedFrame = zip(thisFrame, nextFrame)

    combinedFrame = []

    for frame in zippedFrame:
        z1 = frame
        z1a = z1[0]
        z1b = z1[1]
        z1tuple = (t * z1a['posx'] + y* z1b['posx'], 
                   t * z1a['posy'] + y* z1b['posy'], 
                   t * z1a['posz'] + y* z1b['posz'], 
                   z1a['texu'], z1b['texv'])
        combinedFrame.append(z1tuple)

    combinedFrame = numpy.array(combinedFrame)
    a1 = numpy.empty((len(combinedFrame),), dtype=object)
    a1[:] =[tuple(i) for i in combinedFrame]

    df1 = pandas.DataFrame(thisFrame)
    df2 = pandas.DataFrame(nextFrame)
    df3 = pandas.DataFrame()
    df3[['posx', 'posy', 'posz']] = t * df1[['posx', 'posy', 'posz']] + y* df2[['posx', 'posy', 'posz']]
    df3[['texu', 'texv']] = df1[['texu', 'texv']]
    matrix = numpy.array(df3.as_matrix())
    a2=np.empty((len(matrix),), dtype=object)
    a2[:] =[tuple(i) for i in matrix]