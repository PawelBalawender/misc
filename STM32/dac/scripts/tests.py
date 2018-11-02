#!/usr/bin/env python3
"""
This module implements Q7 fixed-point arithmetic examples
"""
import math


def to_q7(x: float) -> int:
	return math.floor(x * (2 ** 30))


def to_float(x: int) -> float:
	return x / (2 ** 30)
num = 3.1415926
assert math.isclose(to_float(to_q7(num)), num)


def map_sin(x):
	#print('inmap:', x)
	if 0 <= x < math.pi/2:
		return math.sin(x)
	elif math.pi/2 <= x < math.pi:
		return math.sin(math.pi - x)
	elif -math.pi <= x < -math.pi/2:
		return -math.sin(math.pi + x)
	elif -math.pi/2 <= x < 0:
		return -math.sin(-x)

zz = math.sin
def mock(x):
	#print('inmock:', x)
	assert 0 <= x <= math.pi/2
	return zz(x)
math.sin = mock
for i in [-math.pi, -3, -2, -1, 0, 0.2, 0.3, 1, 2, 2.7, 3, math.pi-1e-10]:
	#print('test:', i)
	#print('sin:', zz(i))
	#print('map_sin:', map_sin(i))
	assert math.isclose(zz(i), map_sin(i), abs_tol=1e-8)


def map_cos(x):
	# map to sin
	x += math.pi/2
	# wrap
	if x > math.pi:
		x -= math.pi * 2
	return map_sin(x)

for i in [-math.pi, -3, -2, -1, 0, 0.2, 0.3, 1, 2, 2.7, 3, math.pi-1e-10]:
	#print('test:', i)
	#print('cos:', math.cos(i))
	#print('map_cos:', map_cos(i))
	assert math.isclose(math.cos(i), map_cos(i), abs_tol=1e-8)
