#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import matplotlib.pyplot as plt
import numpy as np
import serial
import time

xlim = 1000

plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111)
ax.set_xlim(0, xlim)
ax.set_ylim(0, 4095)

xs = np.arange(xlim)
ys = np.zeros(xlim)
points, = ax.plot(xs, ys, 'ro')

x = 0
while x < xlim:
    ys[x] = x
    points.set_ydata(ys)
    x += 50
    plt.pause(0.0001)

plt.show()

