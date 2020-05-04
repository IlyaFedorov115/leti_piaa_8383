import os
DIR = 'Tests'
tests = os.listdir(DIR)
for i in range(len(tests)):
    Path = DIR + '/' + tests[i]
    print('Test ', i+1 , ': ', Path, sep="")
    print(*open(Path, "r").readlines(), sep="")
    os.system('./lab3 < ' + Path)