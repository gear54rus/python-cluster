__author__ = 'Crazy_000'


import subprocess
proc = subprocess.Popen(['C:\\Python34\\python.exe', 'generateInput.py', '0', '1000000', '30000', 'numbers.txt'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT
                        )
result, buff = proc.communicate()

proc = subprocess.Popen(['C:\\Python34\\python.exe', 'sorting.py', 'numbers.txt'],
                             shell=False,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT
                        )
result, buff = proc.communicate()

f = open('log.txt','wb')

f.write(result)
f.close()