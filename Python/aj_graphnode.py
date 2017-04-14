
# -*- coding: utf-8 -*-
"""
The base node used by graph

@author: Adrian Lim Xuan Wei
"""

class GraphKey():
    def __init__(self):
        self.Note = 0
        self.Amplitude = 0.0

    def __eq__(self, other):
        if not isinstance(other, GraphKey):
            return False
        if self.Note != other.Note:
            return False
        epsilon = 0.01
        delta = self.Amplitude - other.Amplitude
        return abs(delta) < epsilon

class GraphNode():
    def __init__(self):
        self.key = GraphKey()
        self.child = []
        self.data = []

class SearchGraphNode():
    def __init__(self):
        self.cost = 0.0
        self.key = []
        self.node = None

def GetKeyResult(left, right):
    return 0.0

def SearchGraph(baseNode, key):

    closeSet = []
    openSet = []

    baseSearch = SearchGraphNode()
    baseSearch.node = baseNode

    openSet.append(baseSearch)

    while len(openSet) > 0:
        node = openSet.pop()
        keyIndex = len(node.key)
        currentKey = key[keyIndex]

        score = GetKeyResult(node.node.key, currentKey)

    return []

def InsertToGraph(baseNode, key, value):
    if len(key) <= 0:
        baseNode.data = value
        return

    currentKey = key[0]
    nextKey = key[1:]

    for node in baseNode.child:
        if node.key == currentKey:
            InsertToGraph(node, nextKey, value)
            return

    newNode = GraphNode()
    newNode.key = currentKey
    InsertToGraph(newNode, nextKey, value)
    baseNode.child.append(newNode)

def CreateGraphKey(node, amplitude):
    newKey = GraphKey()
    newKey.Note = node
    newKey.Amplitude = amplitude
    return newKey


if __name__ == "__main__":
    rootNode = GraphNode()
    
    keyA = []
    keyA.append(CreateGraphKey('a', 0.5))
    keyA.append(CreateGraphKey('b', 0.65))
    keyA.append(CreateGraphKey('c', 0.75))
    
    InsertToGraph(rootNode, keyA, "dataA")

    keyB = []
    keyB.append(CreateGraphKey('a', 0.5))
    keyB.append(CreateGraphKey('b', 0.65))
    keyB.append(CreateGraphKey('d', 0.75))
    
    InsertToGraph(rootNode, keyB, "dataB")

    SearchGraph(rootNode, keyA)

    x = 0
    y = x