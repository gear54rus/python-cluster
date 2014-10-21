__author__ = 'DNS'
import sys

f = open(sys.argv[1])
numbers = f.readlines()
f.close()
numbers = list(map(int,numbers))
numbers.sort()

for num in numbers:
    print(int(num))