# -*- coding: utf-8 -*-
"""
Created on Sun May 29 17:05:11 2016
@author: Adrian Lim Xuan Wei
"""

import numpy as np

def readBinaryFile(fileName):
    frame_chunk = 1347
    
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
    
    data['posx'] = data['posx'] * 3
    data['posy'] = data['posy'] * 3
    data['posz'] = data['posz']/100
    
    num_frames = len(data) / frame_chunk
    data_split = np.split(data, num_frames)
    x = 10
    return data_split

def readIndicesFile():
    fileName = '../resources/indices.apf'
    
    f = open(fileName, 'rb')
    
    data = np.fromfile(f, dtype=np.uint32)
    
    return data

if __name__ == "__main__":
    fileName = './face.apf'
    data = readBinaryFile(fileName)
    
    print data

    #indices = readIndicesFile()
    
    #d = data[0]
    #d_posx = d['posx']
    
    #d = data[0]
    #d_cut = d[['posx', 'posy', 'posz']]
    #d_rearranged = d_cut[indices]