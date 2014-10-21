__author__ = 'Crazy_000'

import sys
import subprocess
pythonPath = sys.executable

proc = subprocess.Popen([pythonPath, 'generateInput.py', '80000000', '0', '8000000000', 'input.txt'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT
                        )
result, buff = proc.communicate()

proc = subprocess.Popen([pythonPath, 'brut.py', 'input.txt'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT
                        )
result, buff = proc.communicate()

f = open('log.txt', 'wb')
f.write(result)
f.close()