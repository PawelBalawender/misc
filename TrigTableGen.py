#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import numpy as np

SAMPLES_NUM = 256


def gen_sin() -> np.ndarray:
    xs = np.linspace(0, np.pi / 2, SAMPLES_NUM)
    return np.sin(xs)


def gen_atan2() -> list:
    xs = np.linspace(1, -1, SAMPLES_NUM // 2 + 1)  # +1 to make it evenly spaced, with 0
    xs = np.concatenate((xs, xs[::-1][1:-1]))

    ys = np.linspace(1, -1, SAMPLES_NUM // 2 + 1)
    ys = np.concatenate((ys, ys[::-1][1:-1]))
    ys = np.roll(ys, SAMPLES_NUM // 4)
    assert len(ys) == len(xs) == SAMPLES_NUM

    values = [np.arctan2(y, xs) for y in ys]
    return values


# np.set_printoptions(precision=7,
#                     formatter={'float': '{: 1.7f},'.format},
#                     linewidth=80-4,
#                     suppress=True)

atan2_table = gen_atan2()
for i in atan2_table:
    print(i)
