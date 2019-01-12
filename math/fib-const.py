"""
This module computes consecutive Fibonacci numbers
with O(1) time and memory complexity
"""
from functools import lru_cache
import math


def fib(n):
    lambd_1 = 2 / (math.sqrt(5) - 1)
    lambd_2 = 2 / (-math.sqrt(5) - 1)
    return math.ceil((lambd_1 ** n - lambd_2 ** n) / math.sqrt(5))


@lru_cache(maxsize=128)
def fib_rec(n):
    if n == 0: return 0
    elif n == 1: return 1
    return fib_rec(n-2) + fib_rec(n-1)


for i in range(128):
    try:
        assert fib(i) == fib_rec(i)
    except AssertionError:
        print(fib(i), fib_rec(i))
        break
    # print(fib(i), fib_rec(i))

