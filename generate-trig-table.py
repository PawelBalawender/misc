#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import numpy as np

SAMPLES_NUM = 256


def gen_sin() -> np.ndarray:
    xs = np.linspace(0, np.pi / 2, SAMPLES_NUM)
    return np.sin(xs)


def gen_atan2() -> list:
    # +1 to include 0
    xs = np.linspace(0, 1, SAMPLES_NUM + 1)
    ys = (1 - xs**2) ** (1/2)
    values = np.arctan2(ys, xs)
    return values


np.set_printoptions(precision=7,
                    formatter={'float': '{: 1.7f},'.format},
                    linewidth=80-4,
                    suppress=True)

atan2_table = gen_atan2()
print(atan2_table)
