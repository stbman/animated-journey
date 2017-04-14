# -*- coding: utf-8 -*-
"""

@author: Adrian Lim Xuan Wei
"""

import numpy 
import matplotlib.pyplot as plt

def PlotAudio(audioData, threshold, fileName):
    plt.clf()
    x = numpy.arange(0, len(audioData), 1)
    plt.plot(x, audioData)
    plt.axhline(y=threshold, linewidth=3, color='g')

    figure = plt.gcf() # get current figure
    figure.set_size_inches(10, 8)

    #plt.show()
    plt.savefig('./audioData/' + str(fileName) + '.png')