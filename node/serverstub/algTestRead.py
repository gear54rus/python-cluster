import time
import sys
tempStdOut = sys.stdout
sys.stdout = open('log', 'w')
number = 25
number = int(number)
numberofcycles = 10000
for i in range(0, numberofcycles):
    number = number + 1
    print(str(number))
time.sleep(2)  # sleep for 5 seconds
print('done')
sys.stdout.close()
sys.stdout = tempStdOut