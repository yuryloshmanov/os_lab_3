import sys
import random

if len(sys.argv) != 2:
    exit(1)

for i in range(0, int(sys.argv[1])):
    size = random.randint(1, 2)
    if size == 2:
        size = random.randint(1, 5)

    for j in range(0, size):
        print(f'{random.randint(1, 10)}', end=' ')
    print('\n')
