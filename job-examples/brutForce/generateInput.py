__author__ = 'Crazy_000'

import hashlib
import struct
import sys
import random

hasher = hashlib.md5()

tar = int(sys.argv[1])
start = int(sys.argv[2])
stop = int(sys.argv[3])
output = sys.argv[4]

if  tar == 0:
    #herovo randomit
    tar = random.randint(start, stop)

hasher.update(struct.pack('>I', tar))

hash = hasher.hexdigest()

f = open(output, 'w')
f.writelines([hash + '\n', str(start) + '\n', str(stop)])
f.close()