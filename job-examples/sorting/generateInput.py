__author__ = 'DNS'
import sys
import random
count = int(sys.argv[3])
min = int(sys.argv[1])
max = int(sys.argv[2])
f = open(sys.argv[4], 'w')
nextpercent = 0
nums = []
for i in range (0,count):
    percent = float(i)/count * 100
    step = 5
    if percent >= nextpercent:
        print(str(percent) + '%')
        nextpercent = nextpercent + step
    num = random.randint(min, max)
    nums.append(str(num) +'\n')
f.writelines(nums)
f.close()