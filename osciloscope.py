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

x = 0
t = time.time()
while x < xlim:
    plt.plot(x, x, 'ro')
    x += 50
    plt.pause(0.0001)
print(time.time()-t)
plt.show()

