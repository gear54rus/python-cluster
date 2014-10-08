import time
import sys


sys.stdout = open('log', 'w')
sys.stderr = sys.stdout

f = open('input', 'rb')
number = f.read(1)
number = int(number)
numberofcycles = 10000
for i in range(0, numberofcycles):
    number = number + 1
    print(str(number))
time.sleep(2)  # sleep for 5 seconds
print('done')

f.close()