__author__ = 'Crazy_000'


import subprocess
proc = subprocess.Popen(['C:\\Python34\\python.exe', 'generateInput.py', '80000000', '0', '8000000000', 'input.txt'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT
                        )
result, buff = proc.communicate()

proc = subprocess.Popen(['C:\\Python34\\python.exe', 'brut.py', 'input.txt'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT
                        )
result, buff = proc.communicate()

f = open('log.txt', 'wb')
f.write(result)
f.close()