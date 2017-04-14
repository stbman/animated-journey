 # -*- coding: utf-8 -*-
"""
Created on Wed Jun 01 09:32:50 2016

OpenGL Widget for PyQt

@author: Adrian Lim Xuan Wei
"""

# PyQt4 imports
from PyQt4.QtOpenGL import QGLWidget

# PyOpenGL imports
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GL import shaders
from OpenGL.arrays import vbo

from PIL import Image

import numpy

# Custom graphics imports
import aj_binaryreader
import aj_shadersCustom
import aj_globalvar
import aj_camera
import aj_graphprocessor

from aj_vecmath import *
from aj_spatial import *
import numpy as np
from scipy.io import wavfile


class OpenGlWidget(QGLWidget):
    
    def initializeGL(self):
        self.width, self.height = 450, 450
        self.setMinimumSize(self.width, self.height)
    
        vertexShader = shaders.compileShader(aj_shadersCustom.vertexShader, GL_VERTEX_SHADER)
    
        fragmentShader = shaders.compileShader(aj_shadersCustom.fragmentShader, GL_FRAGMENT_SHADER)
    
        self.shaderProgram = shaders.compileProgram(vertexShader, fragmentShader)
        
        self.vertexData = []
        self.anchorFrame = []
        #vertexData = self.GetInterpolatedFrame(0.0)
        vertexData = aj_globalvar.g_AnchorFace
        self.frameCount = aj_globalvar.g_BinaryReader.GetFrameCount()

        self.numVertices = len(vertexData)
        
        # Core OpenGL requires that at least one OpenGL vertex array be bound
        self.VAO = glGenVertexArrays(1)
        glBindVertexArray(self.VAO)
    
        # Need VBO for triangle vertices and texture UV coordinates
        self.VBO = vbo.VBO(vertexData)
        self.VBO.bind()

        indicesData = aj_globalvar.g_BinaryReader.indices
        self.numIndices = len(indicesData)
        self.IBO = vbo.VBO(indicesData, target=GL_ELEMENT_ARRAY_BUFFER)
        self.IBO.bind()
    
        # enable array and set up data
        positionAttrib = glGetAttribLocation(self.shaderProgram, 'position')
        coordsAttrib = glGetAttribLocation(self.shaderProgram, 'texCoords')
    
        glEnableVertexAttribArray(0)
        glEnableVertexAttribArray(1)
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 20,
            None)
        # the last parameter is a pointer
        glVertexAttribPointer(coordsAttrib, 2, GL_FLOAT, GL_TRUE, 20,
            ctypes.c_void_p(12))
    
        # load texture and assign texture unit for shaders
        self.sampleTexture = self.loadTexture(aj_globalvar.rootDir + '/face.jpg')
        #self.sampleTexture = self.loadTexture('./resources/face.bmp')
        self.texUnitUniform = glGetUniformLocation(self.shaderProgram, 'texUnit')
    
        self.worldMatrix = glGetUniformLocation(self.shaderProgram,"WorldMtx")
        self.viewMatrix = glGetUniformLocation(self.shaderProgram,"ViewMtx")
        self.projectionMatrix = glGetUniformLocation(self.shaderProgram,"ProjectionMtx")

        self.camera = aj_camera.Camera()
        #self.camera.set_position(v3(0,0,5))
        self.camera.set_position(v3(0,0,3))

        # Finished
        glBindBuffer(GL_ARRAY_BUFFER, 0)
        glBindVertexArray(0)

    def GetFaceFrameNormally(self):
        currentRatio = aj_globalvar.audioThread.GetCompletionRatio()
        vertexDataCurrent = []

        if aj_globalvar.g_SoundRecorder == 0 or not aj_globalvar.g_SoundRecorder.IsReady():
            playTime = aj_globalvar.audioThread.GetAudioTime()
            #this function is super slow :(
            #faceFrame = aj_graphprocessor.GetFaceFrame(self.framerate, self.audioData, currentRatio * playTime, False)
            #if dont want threshold, put None
            faceFrame = aj_graphprocessor.GetFaceFrame(aj_globalvar.audioThread.framerate, aj_globalvar.audioThread.audioData, currentRatio * playTime, aj_globalvar.audioThread.threshold)
            #faceFrame = aj_graphprocessor.GetFaceFrame(aj_globalvar.audioThread.framerate, aj_globalvar.audioThread.audioData, currentRatio * playTime, None)
            if faceFrame != []:
                vertexDataCurrent = faceFrame.copy()
        else:
            faceFrame = aj_globalvar.g_SoundRecorder.GetCurrentFaceData()
            if faceFrame != []:
                vertexDataCurrent = faceFrame.copy()
        
        if self.vertexData == []:
            #vertexData = aj_graphprocessor.GetFaceFrame(self.framerate, self.audioData, playTime, False)
            vertexData = aj_graphprocessor.GetFaceFrame(aj_globalvar.audioThread.framerate, aj_globalvar.audioThread.audioData, playTime, None)
            self.vertexData = vertexData.copy()
            self.anchorFrame = vertexData.copy()

        if vertexDataCurrent == []:
            vertexDataCurrent = self.anchorFrame

        if self.vertexData == []:
            return

        # generate the array of best index

        for j in xrange(len(aj_globalvar.g_InterpolationIndex)):
            i = aj_globalvar.g_InterpolationIndex[j]
            ratio = aj_globalvar.g_InterpolationData[i]
            inverseRatio = 1.0 - ratio
        
            self.vertexData[i][0] = (vertexDataCurrent[i][0] * ratio) + (self.vertexData[i][0] * inverseRatio)
            self.vertexData[i][1] = (vertexDataCurrent[i][1] * ratio) + (self.vertexData[i][1] * inverseRatio)
            self.vertexData[i][2] = (vertexDataCurrent[i][2] * ratio) + (self.vertexData[i][2] * inverseRatio)
        
            vertexDataCurrent[i][0] = self.vertexData[i][0]
            vertexDataCurrent[i][1] = self.vertexData[i][1]
            vertexDataCurrent[i][2] = self.vertexData[i][2]

        return vertexDataCurrent

    def GetFaceFrameFromBase(self):
        currentRatio = aj_globalvar.audioThread.GetCompletionRatio()
        totalFrame = len(aj_globalvar.g_originalFaceData) - 1
        index = int(totalFrame * currentRatio)
        #index = (index / 2) * 2;
        vertexDataCurrent = aj_globalvar.g_originalFaceData[index][0].copy()
        return vertexDataCurrent

    def GetFaceDrameFromTarget(self):
        currentRatio = aj_globalvar.audioThread.GetCompletionRatio()
        totalFrame = len(aj_globalvar.g_generatedFaceData) - 1
        index = int(totalFrame * currentRatio)
        vertexDataCurrent = aj_globalvar.g_generatedFaceData[index].copy()

        if len(self.vertexData) == 0:
            self.vertexData = aj_globalvar.g_originalFaceData[0][0].copy();
            self.anchorFrame = aj_globalvar.g_originalFaceData[0][0].copy();
            amplitudeData = np.abs(aj_globalvar.g_generatedFaceAmplitude)
            sortedData = np.sort(amplitudeData)
            #median = np.median(amplitudeData)
            #min = np.min(amplitudeData)
            #self.clampedVolume = (median + min) * 0.5;
            clampIndex = int(len(sortedData) * 0.1)
            self.clampedVolume = sortedData[clampIndex]
            peakIndex = int(len(sortedData) * 0.9)
            self.peakVolume = sortedData[peakIndex]

        ratioMultiplier = 0.5
        if np.abs(aj_globalvar.g_generatedFaceAmplitude[index]) <= self.clampedVolume:
            vertexDataCurrent = self.anchorFrame;
            ratioMultiplier = 1.0

        if np.abs(aj_globalvar.g_generatedFaceAmplitude[index]) >= self.peakVolume:
            ratioMultiplier = 1.0

        for j in xrange(len(aj_globalvar.g_InterpolationIndex)):
            i = aj_globalvar.g_InterpolationIndex[j]
            ratio = aj_globalvar.g_InterpolationData[i] * aj_globalvar.g_InterpolationData[i]
            ratio = ratio * ratioMultiplier
            inverseRatio = 1.0 - ratio
        
            self.vertexData[i][0] = (vertexDataCurrent[i][0] * ratio) + (self.vertexData[i][0] * inverseRatio)
            self.vertexData[i][1] = (vertexDataCurrent[i][1] * ratio) + (self.vertexData[i][1] * inverseRatio)
            self.vertexData[i][2] = (vertexDataCurrent[i][2] * ratio) + (self.vertexData[i][2] * inverseRatio)
        
            vertexDataCurrent[i][0] = self.vertexData[i][0]
            vertexDataCurrent[i][1] = self.vertexData[i][1]
            vertexDataCurrent[i][2] = self.vertexData[i][2]

        return vertexDataCurrent

    def paintGL(self):
        #self.framerate , self.audioData = wavfile.read(aj_globalvar.audioThread.wavFileName)

        # get base face frame
        # get target face frame
        if aj_globalvar.g_RenderMode == 0:
            vertexDataCurrent = self.GetFaceFrameNormally()
        if aj_globalvar.g_RenderMode == 1:
            vertexDataCurrent = self.GetFaceFrameFromBase()
        if aj_globalvar.g_RenderMode == 2:
            vertexDataCurrent = self.GetFaceDrameFromTarget()


        resultMx = m44Identity();

        glClearColor(0, 0, 0, 1)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        # active shader program
        glUseProgram(self.shaderProgram)

        self.VBO.bind()
        self.IBO.bind()

        glUniformMatrix4fv(self.worldMatrix ,1 ,GL_FALSE, resultMx);
        
        glUniformMatrix4fv(self.viewMatrix ,1 ,GL_FALSE, self.camera.get_camera_matrix());

        glUniformMatrix4fv(self.projectionMatrix ,1 ,GL_FALSE, self.camera.get_projection_matrix());

        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataCurrent.nbytes, vertexDataCurrent)
        glBindBuffer(GL_ARRAY_BUFFER, 0)
        
        try:
            # Activate texture
            glActiveTexture(GL_TEXTURE0)
            glBindTexture(GL_TEXTURE_2D, self.sampleTexture)
            glUniform1i(self.texUnitUniform, 0)
            
            # Activate array
            glBindVertexArray(self.VAO)
            
            # draw triangle
            if (aj_globalvar.useMesh):
                glDrawElements(GL_TRIANGLES, self.numIndices, GL_UNSIGNED_INT, None)
            else:
                glDrawElements(GL_POINTS, self.numIndices, GL_UNSIGNED_INT, None)
            
            
        finally:
            glBindVertexArray(0)
            glUseProgram(0)
  
    def loadTexture(self, path):
        img = Image.open(path).transpose(Image.FLIP_TOP_BOTTOM)
        #img = Image.open(path)
        img_data = numpy.fromstring(img.tostring(), numpy.uint8)
        width, height = img.size

        texture = glGenTextures(1)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
        glBindTexture(GL_TEXTURE_2D, texture)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, img_data)
        glGenerateMipmap(GL_TEXTURE_2D)
        return texture
  
    def resizeGL(self, width, height):
        self.width, self.height = width, height
        glViewport(0, 0, self.width, self.height)


