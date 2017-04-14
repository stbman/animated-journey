# -*- coding: utf-8 -*-
"""
Created on Wed Jun 01 09:12:50 2016

@author: Adrian Lim Xuan Wei
"""

vertexShader = """
#version 330
layout (location=0) in vec3 position;
layout (location=1) in vec2 texCoords;
uniform mat4 WorldMtx;
uniform mat4 ViewMtx;
uniform mat4 ProjectionMtx;
out vec2 theCoords;
void main()
{
    vec4 worldPos = WorldMtx * vec4(position, 1);
    vec4 viewPos = ViewMtx * worldPos;
    vec4 projPos = ProjectionMtx * viewPos;
    gl_Position = projPos;
    theCoords = texCoords;
}
"""

fragmentShader = """
#version 330
uniform sampler2D texUnit;
in vec2 theCoords;
out vec4 outputColour;
void main()
{
    outputColour = texture(texUnit, theCoords);
}
"""

      