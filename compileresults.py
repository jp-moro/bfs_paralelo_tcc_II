import sys

def readFile(fileName):
    dataSearch = []
    fileObject = open(fileName, 'r')
    fileData = fileObject.read().splitlines()
    fileObject.close()
    for element in fileData:
        temp = element.split("::")
        temp[1] = float(temp[1])
        dataSearch.append(temp)
    return dataSearch

def evaluateMean(data, nbElements):
    dataMean = []

    for i in xrange(0, len(data), nbElements):
        add = 0.0
        ip = data[i][0]
        for j in range(nbElements):
            add += data[i+j][1]

        mean = add/nbElements
        dataMean.append(str(mean))

    return dataMean

def flushFile(fileName, dataMean):
#    fileNameOut = fileName[4:-4]
#    fileNameOut += "Media.txt"
    fileNameOut = "MEDIA" + fileName
    fileObject = open(fileNameOut, 'w')

    for element in dataMean:
        fileObject.write(element + "\n")

    fileObject.close()

fileName = sys.argv[1]
nbNumbers = int(sys.argv[2])

dataSearch = readFile(fileName)

dataMean = evaluateMean(dataSearch, nbNumbers)

flushFile(fileName, dataMean)

