#!/usr/bin/env python3

a = []
for left in [1]:
    for right in [0, 1]:
        for front in [0, 1]:
            for velocity in [3, 4]:
                a += [left << 5 | right << 4 | front << 3 | velocity]

b = []
for left in [0, 1]:
    for right in [1]:
        for front in [0, 1]:
            for velocity in [3, 4]:
                b += [left << 5 | right << 4 | front << 3 | velocity]

c = []
for left in [1]:
    for right in [1]:
        for front in [0, 1]:
            for velocity in [2, 3, 4]:
                c += [left << 5 | right << 4 | front << 3 | velocity]

d = []
for left in [1]:
    for right in [1]:
        for front in [0, 1]:
            for velocity in [3, 4]:
                d += [left << 5 | right << 4 | front << 3 | velocity]

e = []
for left in [0, 1]:
    for right in [0, 1]:
        for front in [1]:
            for velocity in [1, 2, 3, 4]:
                e += [left << 5 | right << 4 | front << 3 | velocity]

f = []
for left in [0, 1]:
    for right in [0, 1]:
        for front in [0, 1]:
            for velocity in [4]:
                f += [left << 5 | right << 4 | front << 3 | velocity]
x = len(f)
f.remove(0b000100)
assert len(f) == x - 1

z = set(a + b + c + d + e + f)
print(len(z))

