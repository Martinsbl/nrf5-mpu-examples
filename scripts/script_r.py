newFileName = 'mpu9255_register_map_test_r.h'
oldFileName = 'mpu9255_register_map_unformated.h'

with open(oldFileName,'r') as oldFile: 
    with open(newFileName,'w') as newFile: 
        previousLine = ""
        for line in oldFile:
            # Check if two first letters in line is hex number
            if (line[0] == '#'):
                if (previousLine[0] != '#' and previousLine != ""):
                    newFile.write('\n')   
                newFile.write(line)
            else:
                newFile.write(previousLine.rstrip())

            previousLine = line