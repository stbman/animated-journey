# -*- coding: utf-8 -*

"""

Machine learning of parameters of manhattan distance 
Multilabel machine learning

@author: Adrian Lim Xuan Wei
"""

import numpy
import matplotlib.pyplot as plt

import aj_globalvar
import aj_speechnote
import aj_speechprocessor

ENUM_OFFSET = 2
NUM_SPEECH_NOTES = aj_speechnote.Note['Length'].value - ENUM_OFFSET
FUNDAMENTAL_FREQ = aj_speechnote.fundamentalFreq

'''
Transforms the speech data to fit scikit learn format 

Returns X, Y in the form [[X],[X]] [[Y],[Y]]
'''
def transformData():
    speechData = aj_globalvar.allSpeechNotesArray
    print "Speech Data length: " + str(len(speechData))

    all_Y = []
    all_X = []

    for speechnote in speechData:
        Y = [0] * NUM_SPEECH_NOTES
        start = speechnote.start
        peak = speechnote.peak
        end = speechnote.end
        Y[start.value - ENUM_OFFSET] = 1 
        Y[peak.value - ENUM_OFFSET]  = 1
        Y[end.value - ENUM_OFFSET]   = 1

        X = [speechnote.startAmp, speechnote.peakAmp, speechnote.endAmp,
             speechnote.startToPeakTime, speechnote.peakToEndTime,
             FUNDAMENTAL_FREQ[start],
             FUNDAMENTAL_FREQ[peak],
             FUNDAMENTAL_FREQ[end]
            ]

        all_Y.append(Y)
        all_X.append(X)

    return all_X, all_Y

'''
Transforms the data to fit multiLayerBinarizer 
'''
def multiLabelTransform():
    from sklearn.preprocessing import MultiLabelBinarizer

    speechData = aj_globalvar.allSpeechNotesArray
    print "Speech Data length: " + str(len(speechData))

    all_Y = []
    all_X = []

    for speechnote in speechData:
        Y = []
        start = speechnote.start
        peak = speechnote.peak
        end = speechnote.end
        Y.append(start.value - ENUM_OFFSET)
        Y.append(peak.value - ENUM_OFFSET)
        Y.append(end.value - ENUM_OFFSET)

        X = [speechnote.startAmp, speechnote.peakAmp, speechnote.endAmp,
             speechnote.startToPeakTime, speechnote.peakToEndTime,
             FUNDAMENTAL_FREQ[start],
             FUNDAMENTAL_FREQ[peak],
             FUNDAMENTAL_FREQ[end]
            ]

        all_Y.append(Y)
        all_X.append(X)

    transform_y = MultiLabelBinarizer().fit_transform(all_Y)

    return all_X, transform_y

'''
Cross validation

Input: 
- X: [[X], [X]]
- Y: [[Y], [Y]]
- test ratio float between (0, 1) to divide the testing data into
Output: X_train, X_test, Y_train, Y_test
'''
def performCrossValidation(all_X, all_Y, testRatio=0.4):
    from sklearn import cross_validation
    X_train, X_test, Y_train, Y_test = cross_validation.train_test_split( \
        all_X, all_Y, test_size=testRatio, random_state=0)

    return X_train, X_test, Y_train, Y_test

'''
Linear support vector classifier 

http://scikit-learn.org/stable/modules/generated/sklearn.svm.LinearSVC.html#sklearn.svm.LinearSVC
'''

def linearSVC(X, Y):
    from sklearn.multiclass import OneVsRestClassifier
    from sklearn.svm import LinearSVC

    ovr = OneVsRestClassifier(LinearSVC(random_state=0)).fit(X, Y)

    performTenFoldValidation(ovr, X, Y, 'Linear Support Vector Classifier')

'''
Gaussian Naive Bayes 

http://scikit-learn.org/stable/modules/generated/sklearn.naive_bayes.GaussianNB.html#sklearn.naive_bayes.GaussianNB
'''
def gaussianNaiveBayes(X, Y):
    from sklearn.multiclass import OneVsRestClassifier
    from sklearn.naive_bayes import GaussianNB

    ovr = OneVsRestClassifier(GaussianNB()).fit(X, Y)
    performTenFoldValidation(ovr, X, Y, 'Gaussian Naive Bayes')

'''
Logistic Regression 

http://scikit-learn.org/stable/modules/generated/sklearn.linear_model.LogisticRegression.html#sklearn.linear_model.LogisticRegression
'''
def logisticRegression(X, Y):
    from sklearn.multiclass import OneVsRestClassifier
    from sklearn.linear_model import LogisticRegression

    ovr = OneVsRestClassifier(LogisticRegression()).fit(X, Y)
    performTenFoldValidation(ovr, X, Y, 'Logistic Regression')


def performTenFoldValidation(clf, X, Y, name):
    scoreArray = []

    for i in xrange(1, 10):
        X_train, X_test, Y_train, Y_test = performCrossValidation(X, Y, 0.4)
        score = calculateClassifierScore(clf, X_test, Y_test)
        score = numpy.round(score * 100, 2)

        scoreArray.append(score)

    print numpy.mean(scoreArray)

    # Plot the scores
    x = numpy.arange(1, 10)
    plt.clf()
    plt.plot(x , scoreArray)
    plt.title(name)
    plt.ylabel('Accuracy (%)')
    plt.xlabel('Trial Number')

    plt.show()
'''
Random Forest Classifier 
http://scikit-learn.org/stable/modules/generated/sklearn.ensemble.RandomForestClassifier.html#sklearn.ensemble.RandomForestClassifier
'''
def randomForest(X_train, X_test, Y_train, Y_test):
    from sklearn.ensemble import RandomForestClassifier

    # Optimal somewhere around n=20
    #rfc = RandomForestClassifier(n_estimators=20)
    #rfc.fit(X_train, Y_train)
    #forestWeights = rfc.feature_importances_
    #print forestWeights

    scoreArray = []

    for n in xrange(1, 100, 10):
        rfc = RandomForestClassifier(n_estimators=n)
        rfc.fit(X_train, Y_train)
        forestWeights = rfc.feature_importances_
    
        score = calculateClassifierScore(rfc, X_test, Y_test)
        score = numpy.round(score * 100, 2)

        scoreArray.append(score)

    print numpy.mean(scoreArray)

    # Plot the scores
    x = numpy.arange(1, 100, 10)
    plt.clf()
    plt.plot(x , scoreArray)
    plt.title('Random Forest Classifier')
    plt.ylabel('Accuracy (%)')
    plt.xlabel('Number of forests estimators')

    #plt.show()

    #return forestWeights

'''
Bernoulli RBF Neural Network

http://scikit-learn.org/stable/modules/generated/sklearn.neural_network.BernoulliRBM.html#sklearn.neural_network.BernoulliRBM
'''
def neuralNetwork(X_train, X_test, Y_train, Y_test):
    from sklearn.multiclass import OneVsRestClassifier
    from sklearn.neural_network import BernoulliRBM

    model = BernoulliRBM(n_components=2)
    model.fit(X_train, Y_train)

    return model

'''
Classifier helper function

Given classifier, predict output and calculate accuracy 

Input:
- clf, the random forest classifier 
- X_test and Y_test arrays

Output: mean score in %
'''
def calculateClassifierScore(clf, X_test, Y_test):
    output = clf.predict(X_test)

    output_numpy = numpy.array(output)
    y_numpy = numpy.array(Y_test)

    # Count how many elements are the same 
    diffArray = (output_numpy == y_numpy)
    sumArray = numpy.sum(diffArray, axis=1)
    percentageArray = sumArray / float(NUM_SPEECH_NOTES)

    return numpy.mean(percentageArray)

if __name__ == "__main__":
    # Dummy data 
    #X = [[0.2, 3.2, 4.5], [1.2, 3.4, 3.2], [2.3, 4, 5], [3.2, 4.6, 7]]
    #Y = [[0,0,1,1], [0,1,0,1], [1,0,1,1],[1,1,0,0]]
    #
    #x_test = [[0.2,3.0,7.0]]

    aj_speechprocessor.processAllAudioData()
    #all_X, all_Y = transformData()
    
    all_X, all_Y = multiLabelTransform()
    
    X_train, X_test, Y_train, Y_test = performCrossValidation(all_X, all_Y, 0.4)

    #ovr = linearSVC(all_X, all_Y)

    #gaussianNaiveBayes(all_X, all_Y)

    #logisticRegression(all_X, all_Y)

    model = neuralNetwork(X_train, X_test, Y_train, Y_test)

    #forestWeights = randomForest(X_train, X_test, Y_train, Y_test)
    #forestWeights = randomForest(X_train, X_test, Y_train, Y_test)
    
    print "All X length: " + str(len(all_X)) + " All Y length: " + str(len(all_Y))
    print "X Train length: " + str(len(X_train)) + " X Test Length: " + str(len(X_test)) + " Y Train length: " + str(len(Y_train)) + " Y Test length: " + str(len(Y_test))

    #rfc, forestWeights = randomForest(all_X, all_Y)