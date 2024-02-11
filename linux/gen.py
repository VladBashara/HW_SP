import random
import sys

s = 0
string = ''
for i in range(int(sys.argv[4])):
    a = random.uniform(int(sys.argv[2]), int(sys.argv[3]))
    s += a
    string += str(a)
    string += ' '

f = open(sys.argv[1], 'w')
f.write(string)
f.close()