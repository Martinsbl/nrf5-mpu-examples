mpuName = 'mpu60x0'

newFileName = mpuName + '_register_map_ff.h'
oldFileName = mpuName + '_register_map_unformated.h'
with open(oldFileName, 'r') as f:
    with open(newFileName,'w') as g: 
        # previousNumber = 0
        for line in f:
            try:
                # Check if two first letters in line is hex number
                int(line[:2], 16)
                # Check if first letter after hex number is space
                if (line[2] == ' '):
                    # print ('#define 0x' + line[:2])
                    hexNumber = line[:2]
                    decNumber = int(hexNumber, 16)
                    lineLenght = len(line)

                    if decNumber < 100:
                        subString = line[6:lineLenght]
                        spaceIndex = subString.index(' ')
                        defineString = subString[:spaceIndex]
                    else:
                        subString = line[7:lineLenght]
                        spaceIndex = subString.index(' ')
                        defineString = subString[:spaceIndex]
                    string = '{:7} {:26} {:4} {:12}'.format("#define", 'MPU_REG_' + defineString, '0x' + hexNumber, '// Dec ' + str(decNumber)+', ')
                    g.write(string)

                    subString = subString[spaceIndex+1:]
                    if subString[1] == '/':
                        string = '{:5}'.format(subString[:3]+', ')
                        subString = subString[3:]
                    else:
                        string = '{:5}'.format(subString[:1]+', ')
                        subString = subString[1:]
                    g.write(string)

                    spaceIndex = subString.index(' ')


                    g.write(subString)
                else:
                    g.write(line)

            except ValueError:
                g.write(line)
                # print ('Not a number')



with open(mpuName + '_register_map_ff.h', 'r') as inFile:

        data = inFile.readlines()
        lineNumber = 0
        for line in data:
            if data[lineNumber][0] != '#':
                length = len(data[lineNumber])
                data[lineNumber-1] = data[lineNumber-1].replace("\n", "")
                
            lineNumber = lineNumber + 1

with open(mpuName + '_register_map_formated.h', 'w') as outFile: 
    outFile.writelines( data )


with open(mpuName + '_register_map_formated.h', 'r') as f:
    with open(mpuName + '_register_map.h','w') as g: 
        previousRegDec = 0
        for line in f:
            try:
                index = line.index('0x')
                currentRegHex = line[index+2:index+4]
                currentRegDec = int(currentRegHex, 16)
                if currentRegDec - 1 != previousRegDec:
                    g.write('\n' + line)
                else:
                    g.write(line)

                previousRegDec = currentRegDec
            except ValueError:
                print('substring not found')
