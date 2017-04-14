# -*- coding: utf-8 -*-
"""
Created on Wed Jun 01 09:12:50 2016
@author: Adrian Lim Xuan Wei
"""

vertex_shader = """
#version 330
in vec4 position;
void main()
{
   gl_Position = position;
}
"""

fragment_shader = """
#version 330
void main()
{
   gl_FragColor = vec4(1.0f, 1.0f, 0.0f, 0.0f);
}
"""      