#!/usr/bin/env python3
for i in 'Hello, world 123!\n\0':
    print('+' * ord(i) + '.>', end='')
print()

