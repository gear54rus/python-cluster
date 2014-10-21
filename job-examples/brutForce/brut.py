__author__ = 'Crazy_000'

import hashlib
import struct
import sys
f = open(sys.argv[1], 'r')

tarhash = f.readline().replace('\n','')
start = int(f.readline())
stop = int(f.readline())

flag = False

for x in range(start, stop + 1):
    hasher = hashlib.md5()
    hasher.update(struct.pack('>I', x))
    hash = hasher.hexdigest()
    if hash == tarhash:
        print('found a target')
        print(x)
        print('done')
        flag = True
        break
if not flag:
    print('not found')