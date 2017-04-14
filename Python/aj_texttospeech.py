# -*- coding: utf-8 -*-
"""
This file does text to speech using Google Text to Speech API

@author: Adrian Lim Xuan Wei
"""

from gtts import gTTS
import pyaudio
import wave
import subprocess 
import os.path
import numpy

def text_to_speech(text, mp3FileName):
    tts = gTTS(text=text, lang='en')
    tts.save(mp3FileName)

'''
Input: mp3file.mp3 wavfile.wav
'''
    
def mp3_to_wav(mp3FileName, wavFileName):
    if (os.path.isfile(wavFileName)): 
        wavFileNameArray = wavFileName.split('.')
        wavFileName = str(wavFileNameArray[0]) + ' 2.wav'
    
    subprocess.call('./ffmpeg -i ' + mp3FileName + ' ' + wavFileName)

def play_wav_file(wavFileName):
    wf = wave.open(wavFileName, 'rb')

    pa = pyaudio.PyAudio()

    print wf.getframerate()

    # open stream based on the wave object which has been input.
    stream = pa.open(format =
                    pa.get_format_from_width(wf.getsampwidth()),
                    channels = wf.getnchannels(),
                    rate = wf.getframerate(),
                    output = True)

    # read data 
    chunk = 1024
    data = wf.readframes(chunk)

    # play stream (looping from beginning of file to the end)
    while data != '':
        # writing to the stream is what *actually* plays the sound.
        stream.write(data)
        data = wf.readframes(chunk)

    # cleanup stuff.
    stream.close()
    pa.terminate()

def playWavFileFromArr(dataArr):

    pa = pyaudio.PyAudio()

    stream = pa.open(format = pyaudio.paInt16,
                    channels = 1,
                    rate = 16000,
                    output = True)

    data = dataArr.astype(numpy.int16).tostring()

    stream.write(data)

    stream.close()    
    pa.terminate()

'''
Gets duration of wav file in milliseconds
Input: wav file name in string
Output: Duration in milliseconds (float)
'''

def getDurationWavFile(wavFileName):
    #wavFileName = "../sound/textreadings/1/recording2.wav"
    
    wf = wave.open(wavFileName, 'rb')
    framerate = wf.getframerate()
    frames = wf.getnframes()
    
    #print "frame rate: ", framerate

    durationInSecs = frames / framerate
    
    durationInMS = durationInSecs * 1000
    
    return durationInMS

'''
Record audio to a wav file
http://stackoverflow.com/questions/892199/detect-record-audio-in-python
'''
def recordAudio(outAudioFile):
    FORMAT = pyaudio.paInt16
    CHANNELS = 2
    RATE = 44100
    CHUNK = 1024
    RECORD_SECONDS = 5
     
    audio = pyaudio.PyAudio()
     
    # start Recording
    stream = audio.open(format=FORMAT, channels=CHANNELS,
                    rate=RATE, input=True,
                    frames_per_buffer=CHUNK)
    print "recording..."
    frames = []
     
    for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
        data = stream.read(CHUNK)
        frames.append(data)
    print "finished recording"
     
    # stop Recording
    stream.stop_stream()
    stream.close()
    audio.terminate()
     
    waveFile = wave.open(outAudioFile, 'wb')
    waveFile.setnchannels(CHANNELS)
    waveFile.setsampwidth(audio.get_sample_size(FORMAT))
    waveFile.setframerate(RATE)
    waveFile.writeframes(b''.join(frames))
    waveFile.close()

'''
Given a string, convert it to speech
Convert the mp3 file to wav file

Inputs:
- string to be converted to speech: string
- fileName with .wav extension, i.e. test.wav
'''
def processString(speechString, fileName):
    fileNameArray = fileName.split('.wav')
    mp3FileName = str(fileNameArray[0]) + '.mp3'
    wavFileName = str(fileNameArray[0]) + '.wav'

    tts = text_to_speech(speechString, mp3FileName)
    mp3_to_wav(mp3FileName, wavFileName)

if __name__ == "__main__":
    #tts = text_to_speech('penguin is a super penguin', './sound/superpenguin.mp3')
    #mp3_to_wav('./sound/superpenguin.mp3', './sound/superpenguin.wav')
    play_wav_file('./sound/textreadings/1/recording2.wav')