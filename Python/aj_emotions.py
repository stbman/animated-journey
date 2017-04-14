# -*- coding: utf-8 -*-
"""
This extracts emotions from text with the Watson API

@author Adrian Lim Xuan Wei
"""

import json
from watson_developer_cloud import ToneAnalyzerV3Beta

class EmotionAnalyzer():
    def __init__(self):
        self.toneAnalyzerCredentials = {
            "url": "https://gateway.watsonplatform.net/tone-analyzer-beta/api",
            "password": "ntTcGKOVC2P0",
            "username": "d72e1867-14e4-4ad2-a2c0-263cf3241529"
        }
        
        self.toneAnalyzer = ToneAnalyzerV3Beta(
           username = self.toneAnalyzerCredentials["username"],
           password = self.toneAnalyzerCredentials["password"],
           version = '2016-02-11')       
           
    def analyzeEmotion(self, text):
        print "Text to analyse " + text
        analysed_emotion = json.dumps(self.toneAnalyzer.tone(text=text), indent=2)
        #print (analysed_emotion)
        analysed_emotion_json = json.loads(analysed_emotion)
        emotions_array = analysed_emotion_json["document_tone"]["tone_categories"][0]["tones"]
        
        emotion_scores = []
        
        for emotion in emotions_array:
            tone = {}
            tone['tone_name'] = emotion['tone_name']
            tone['score'] = emotion['score']
            emotion_scores.append(tone)
        
        #print emotion_scores    
        return emotion_scores
        
if __name__ == "__main__":
    emotionAnalyzer = EmotionAnalyzer()        
    text = "why"
    emotionAnalyzer.analyzeEmotion(text)
                
    '''
    text = "what are you staring at?"
    
    tone_analyzer = ToneAnalyzerV3Beta(
       username = tone_analyzer_credentials["username"],
       password = tone_analyzer_credentials["password"],
       version = '2016-02-11')
       
    print(json.dumps(tone_analyzer.tone(text=text), indent=2))
    '''