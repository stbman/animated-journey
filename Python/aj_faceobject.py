 # -*- coding: utf-8 -*-
"""
Created on Wed Jun 01 09:32:50 2016

OpenGL Widget for PyQt

@author: Adrian Lim Xuan Wei
"""

from PIL import Image

import aj_globalvar
import aj_shadersCustom

class FaceObject():

    def __init__(self):
        #self.Ver
        x = 0

    def initialize(self):

        vertexShader = shaders.compileShader(aj_shadersCustom.vertexShader, GL_VERTEX_SHADER)
    
        fragmentShader = shaders.compileShader(aj_shadersCustom.fragmentShader, GL_FRAGMENT_SHADER)
    
        self.shaderProgram = shaders.compileProgram(vertexShader, fragmentShader)

        self.vertexData = []
        self.anchorFrame = []

        vertexData = aj_globalvar.g_AnchorFace

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

    def render(self):
        # active shader program
        glUseProgram(self.shaderProgram)

        self.VBO.bind()
        self.IBO.bind()

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