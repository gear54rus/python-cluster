__author__ = 'Crazy_000'


import subprocess
pythonPath = sys.executable
proc = subprocess.Popen([pythonPath, 'generateInput.py', '0', '1000000', '30000', 'numbers.txt'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT
                        )
result, buff = proc.communicate()

proc = subprocess.Popen([pythonPath, 'sorting.py', 'numbers.txt'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT
                        )
result, buff = proc.communicate()

f = open('log.txt','wb')

f.write(result)
f.close()