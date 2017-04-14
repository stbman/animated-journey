# -*- coding: utf-8 -*-
"""
Created on Wed Jun 01 09:32:50 2016
@author: Adrian Lim Xuan Wei
"""
# PyQt4 imports
from PyQt4.QtOpenGL import QGLWidget

# PyOpenGL imports
from OpenGL.GL import *
from OpenGL.GLU import *
import OpenGL.arrays.vbo as glvbo
import OpenGL.GL.shaders

# Custom graphics imports
import shaders

class GLPlotWidget(QGLWidget):
    
    def __init__(self, parent):
        QGLWidget.__init__(self, parent)
        self.width, self.height = 600, 600
        self.setMinimumSize(self.width, self.height)

    def set_data(self, data):
        self.data = data
        self.count = data.shape[0]
        
        self.vbo = glvbo.VBO(self.data)        

    def initializeGL(self):
        """Initialize OpenGL, VBOs, upload data on the GPU, etc.
        """
        glClearDepth(1.0)   
        glClearColor(0,0,0,0)
          
        self.shader = OpenGL.GL.shaders.compileProgram(
            OpenGL.GL.shaders.compileShader(shaders.vertex_shader, GL_VERTEX_SHADER),
            OpenGL.GL.shaders.compileShader(shaders.fragment_shader, GL_FRAGMENT_SHADER)
        )        
        
        #gluPerspective(45.0, 1.33, 0.1, 100.0) 

    def paintGL(self):
        """Paint the scene.
        """
        glClearColor(0,0,0,0)
        
        glUseProgram(self.shader)        
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        
        # Draw outlines only
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)        
        
        self.vbo.bind()
        glEnableClientState(GL_VERTEX_ARRAY)
        glVertexPointer(3, GL_FLOAT, 0, self.vbo)
        glDrawArrays(GL_TRIANGLES, 0, self.count)
        
        self.vbo.unbind()
        glDisableClientState(GL_VERTEX_ARRAY)
        glUseProgram(0)


    def resizeGL(self, width, height):
        """Called upon window resizing: reinitialize the viewport.
        """
        self.width, self.height = width, height
        glViewport(0, 0, self.width, self.height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        glOrtho(-1, 1, 1, -1, -1, 1)