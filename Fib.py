"""
This module computes consecutive Fibonacci numbers
with O(1) time and memory complexity
"""
import math


def fib(n):
    lambd_1 = 2 / (math.sqrt(5) - 1)
    lambd_2 = 2 / (-math.sqrt(5) - 1)
    return round((lambd_1 ** n - lambd_2 ** n) / math.sqrt(5))

for i in range(10):
    print(fib(i))

