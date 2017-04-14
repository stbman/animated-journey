# -*- coding: utf-8 -*-
"""

@author: Adrian Lim Xuan Wei
"""

import aj_binaryreader

import numpy 
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

import scipy
from scipy import ndimage

rootDir = "./EmotionLynnette/"
anchorFaceReader = aj_binaryreader.BinaryReader(rootDir + "anchor.apf")
anchorFace = anchorFaceReader.rearrangeVertices().copy()

interpolationFile = rootDir + "interpolationMap.csv"
interpolationData = numpy.loadtxt(interpolationFile, dtype=float, delimiter=",")
interpolationData = interpolationData

for i in xrange(len(interpolationData)):
    interpolationData[i] *= interpolationData[i] * interpolationData[i] * interpolationData[i]

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

xPos = anchorFace['posx']
yPos = anchorFace['posy']
zPos = anchorFace['posz']

ax.scatter(xPos, zPos, yPos, s=interpolationData*15, c='b')

#ax.view_init(30,0)

plt.show()
