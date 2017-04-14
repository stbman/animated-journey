import scipy
from scipy.fftpack import fft
from scipy.io import wavfile

import numpy 

import aj_speechnote

halfWindow = 256 / 2

'''
Input: index in audio data array: int
Output: fundamental frequency: float 
'''
def findFundamentalFreqOfPoint(audioData, point):
    lengthOfData = len(audioData)

    left = point - halfWindow
    if left < 0:
        left = 0
    right = point + halfWindow
    if right > lengthOfData:
        right = lengthOfData

    truncatedData = audioData[left:right]
    autoCorrData = autocorrelation(truncatedData)

    fftData = scipy.fftpack.fft(autoCorrData)
    halfFFTData = numpy.abs(fftData[:len(autoCorrData)/2+1])    # FFT is symmetrical
    fundamentalFreq = numpy.argmax(halfFFTData)

    return fundamentalFreq

'''
Autocorrelates signal

Input: numpy array signal
Output: autocorrelated numpy array signal
'''
def autocorrelation(signal):
    result = numpy.correlate(signal, signal, mode='full')
    return result[result.size/2:]

def findNoteOfOnePeak(freq):
    note = aj_speechnote.getNoteFromFreq(freq)
    return note