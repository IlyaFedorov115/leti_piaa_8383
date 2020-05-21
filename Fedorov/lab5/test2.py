import os
DIR = 'Tests/Tests2'
tests = os.listdir(DIR)
for i in range(len(tests)):
    Path = DIR + '/' + tests[i]
    print('Test ', i+1 , ': ', Path, sep="")
    print(*open(Path, "r").readlines(), sep="")
    os.system('./lab52 < ' + Path)