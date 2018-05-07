#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
Pol drogi przyspiesza, potem symetrycznie hamuje
"""
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib import animation
import math

fig = plt.figure()
plt.axis('equal')
plt.grid()
ax = fig.add_subplot(111)
ax.set_xlim([-10, 20])
ax.set_ylim([-10, 20])

auto_width = 1.2
auto_height = 0.9
radius = 0.5

patch = patches.Rectangle((-auto_width/2, -auto_height/2), auto_width,
                          auto_height, fc='r')

patch2 = patches.Rectangle((-auto_width/2, -auto_height/2), auto_width/5,
                          auto_height, fc='k')

circle_x = -5
circle_y = -5
patch3 = patches.Circle((circle_x, circle_y), radius, fc='k')


def init():
    ax.add_patch(patch)
    ax.add_patch(patch2)
    ax.add_patch(patch3)
    return patch, patch2, patch3,

V_MAX = 1

auto_v_lin = 0
auto_a_lin = 0
auto_v_ang = 0

auto_x = 0
auto_y = 0
auto_ang = 0

arr = []


def animate():
    patch.set_xy([auto_x-auto_width/2, auto_y-auto_height/2])
    patch2.set_xy([auto_x - auto_width / 2, auto_y - auto_height / 2])
    patch._angle = np.rad2deg(auto_ang)
    patch2._angle = np.rad2deg(auto_ang)

    return patch, patch2, patch3


def integrate(time):
    global auto_x, auto_y, auto_ang, auto_v_lin, arr, auto_v_ang
    auto_x += auto_v_lin * np.cos(auto_ang)
    auto_y += auto_v_lin * np.sin(auto_ang)
    auto_v_lin += auto_a_lin

    auto_v_lin = min( auto_v_lin, V_MAX )
    auto_v_lin = max( auto_v_lin, 0 )

    auto_v_ang = min ( auto_v_ang, np.pi/4)
    auto_v_ang = max ( auto_v_ang, -np.pi/4)
    auto_ang += auto_v_ang
    auto_ang %= 2 * math.pi

    arr += [(time, auto_x, auto_v_lin, auto_a_lin)]

    #auto_ang = (0 if time < 10 else (time - 10) * np.pi / 180)

braked = False
run = True
dest = circle_x, circle_y


def control(time):
    global auto_a_lin, braked, auto_v_ang
    # auto_a_lin = (1) / (circle_x/2)
    #
    # if auto_x >= circle_x/2:
    #    auto_a_lin = -auto_a_lin

    # if (auto_x + auto_width / 2) >= (circle_x - radius)/2:
    #     auto_a_lin = -auto_a_lin

    #auto_v_ang = np.pi/16

    predicted = auto_x + auto_v_lin*2
    if braked:
        auto_a_lin = 0
        print("keep 0")
    elif predicted <= circle_x - radius + 0.3: # safety margin
        auto_a_lin = -auto_v_lin
        print("will brake")
        braked = True
    elif not braked:
        auto_a_lin = 0.001

    dx = circle_x - auto_x
    dy = circle_y - auto_y
    ang = math.atan(dy/dx)

    if -math.pi < ang < 0:
        ang += math.pi
    if dx < 0 <= ang <= (math.pi/2):
        ang += math.pi
    if dy < 0 < dx:
        ang -= math.pi

    if ang < auto_ang:
        auto_v_ang = -0.1
    elif ang == auto_ang:
        auto_v_ang = 0
    else:
        auto_v_ang = 0.1


def check(time):
    return
    if not (auto_x + auto_width / 2) <= ( circle_x - radius ):
        print( (auto_x + auto_width / 2) )
        print( (circle_x - radius ) )
        raise AssertionError


def update(time):
    check(time)
    control(time)
    integrate(time)
    return animate()

anim = animation.FuncAnimation(fig, update,
                               init_func=init,
                               frames=5000,
                               interval=10,
                               blit=True,
                               repeat=False)
                               #repeat_delay=1000)
print(arr)
plt.show()

#fig = plt.figure()
#ax = fig.add_subplot(111)

#ts, xs, vs, ass = [list(i) for i in zip(*arr)]
# for i, j in zip([xs, vs, ass], ['r', 'g', 'b']):
#     ax.plot(ts, i, j)
#ax.plot(ts, [xs, vs, ass] )

#ax.plot(ts, ass, 'k+')

#plt.show()

