#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import decimal

import numpy as np

SAMPLES_NUM = 256
MAX_ROW = 16

xs = np.linspace(0, np.pi / 2, SAMPLES_NUM)
ys = np.sin(xs)

np.set_printoptions(precision=7,
                    formatter={'float': '{: 1.7f},'.format},
                    linewidth=108,
                    suppress=True)

print(ys)
