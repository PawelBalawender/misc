"""
int_{0}^{pi/2} sin^2(x) dx
Calculate this integral numerically, using the trapezoidal rule

int_{0}^{1/pi} sin(1/x) dx
Calculate this integral numerically, using the Gaussian quadrature method
"""
from math import sin, pi

import numpy as np


def integral_trapezoidal_rule(f, a: float, b: float, n: int) -> float:
    area = 0
    dx = (b - a) / n
    h1 = f(0)
    for i in range(1, n + 1):
        h2 = f(i * dx)
        area += 1/2 * (h1 + h2) * dx
        h1 = h2
    return area


def integral_rectangles(f, a: float, b: float, n: int) -> float:
    area = 0
    dx = (b - a) / n
    for i in range(0, n):
        h = f(i * dx)
        area += h * dx
    return area


def integral_gaussian_quadrature(f, a: float, b: float, deg: int) -> float:
    # deg: number of sample points and weights
    x, w = np.polynomial.legendre.leggauss(deg)
    # map [-1, 1] interval to [a, b]
    u = 0.5 * (x + 1) * (b - a) + a
    return sum(w * f(u)) * 0.5 * (b - a)


def func_a(x: float) -> float:
    return sin(x) ** 2


def func_b(x: float) -> float:
    return np.sin(1/x)


print('Expected value of int of (sin x)^2 from 0 to PI/2: PI/4 ~ 0.78539')
print('For 1% acurracy: 0.77754 < x < 0.79325')
n = 80
print(integral_trapezoidal_rule(func_a, 0, pi/2, n))
print(integral_rectangles(func_a, 0, pi/2, n))
print('Exp. val. of int of sin(1/x) from 0 to 1/PI: -Ci(PI) ~ -0.07366')
print(integral_gaussian_quadrature(func_b, 0, 1/pi, 1))

