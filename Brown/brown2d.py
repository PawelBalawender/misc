#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
from random import uniform
import math

import matplotlib.pyplot as plt
from matplotlib.patches import Circle
from matplotlib.animation import FuncAnimation

plt.ion()
fig, ax = plt.subplots()
fig.figsize = (10, 10)
ax.axis('equal')
ax.set_xlim([-6, 6])
ax.set_ylim([-6, 6])

dt = 0.01
kT = 0.2
gamma = 0.1
alpha = math.sqrt(2 *kT / gamma)
unif_max = alpha * math.sqrt(3 * dt)
unif_min = -unif_max

xs, ys = [], []

x = y = 0
c = Circle((x, y), 0.125, color='red')
ax.add_artist(c)
fig.canvas.draw()

T = 250
unifs = [uniform(unif_min, unif_max) for i in range(2*T)]
def update(i):
    global x, y
    old_x = x
    old_y = y
    x += unifs[i*2]
    y += unifs[i*2+1]
    c.center = x, y
    fig.canvas.draw()
    ax.plot([old_x, x], [old_y, y], color='blue', linewidth=0.6)

import numpy as np
anim = FuncAnimation(fig, update, frames=np.arange(0, T))
anim.save('mygif.gif', writer='imagemagick')
plt.show()
