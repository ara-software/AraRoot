#!/usr/bin/python
import sys
import os

def CountNumPedValues(fileName):
    counter = 0
    file = open(fileName, "r")
    for line in file:
        vals = line.split()
        counter = counter + len(vals) - 3
    file.close()
    return counter

if len(sys.argv) < 2:
    print('Usage: %s <pedestalFileName>' % (sys.argv[0])) 
    sys.exit(-1)

fileName = sys.argv[1];

if not (os.path.exists(fileName)):
    print('fileName %s does not exist' % (fileName));
    sys.exit(-1)

pedNumbers = CountNumPedValues(fileName)
goodPedNumber = 8 * 4 * 512 * 64

if pedNumbers != goodPedNumber:
    print('Bad ped file')
    print('number of pedNumbers %i %i' % (pedNumbers, goodPedNumber))
    sys.exit(-1)

sys.exit(0)
