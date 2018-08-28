#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
from collections import namedtuple
from random import uniform, random
import subprocess
import math

# precision etc.
parts = 100
Time = 1000000

# constants
kT = 0.2  # Boltzmann's constant * temperature of the environment
delta_t = 0.0001  # time step

gamma = 1  # coefficient of friction of the particle's environment
alpha = math.sqrt(2 * kT / gamma)  # intensity of noise; alpha = sqrt(2kT/gamma)
switch = 1  # likelihood of switching the potential ON

uniform_max = alpha * math.sqrt(3 * delta_t)
uniform_min = -uniform_max


def normalize(data: list) -> list:
    s = sum(data)
    return [i / s for i in data]


def get_theoretical() -> list:
    theor = []
    for i in range(parts):
        x = -0.8 + i / 100
        if x < 0:
            V = -5/4 * x
        else:
            V = 5 * x
        theor += [math.pow(math.e, -V / kT)]

    return normalize(theor)


def get_simulate() -> list:
    # inits:
    bins = [0 for i in range(parts)]

    x = 0
    x_prim = x - math.floor(x + 0.8)  # x'(x) = x - [x - (a - 1)]

    # aux.
    neg_force = 5/4 / gamma * delta_t
    pos_force = -5 / gamma * delta_t

    for i in range(Time):
        x += uniform(uniform_min, uniform_max)
        if random() < switch:  # if the potential is ON
            if x_prim < 0:
                x += neg_force
            else:
                x += pos_force
        x_prim = x - math.floor(x + 0.8)
        bins[int((x_prim + 0.8) * 100)] += 1
    return normalize(bins)

def get_drift(switch=switch):
    particles_num = 1
    # Time = 100000

    neg_force = 5/4/gamma*delta_t
    pos_force = -5/gamma*delta_t

    xs = []
    for i in range(particles_num):
        x = 0
        x_prim = x - math.floor(x + 0.8)
        for t in range(Time):
            c+=1
            x += uniform(uniform_min, uniform_max)
            if random() < switch:  # if the potential is ON
                if x_prim < 0:
                    x += neg_force
                else:
                    x += pos_force
            x_prim = x - math.floor(x + 0.8)
        xs += [x]
    return xs

theor = get_theoretical()
sim = get_simulate()

# plot; x(t)
if False:
    import matplotlib.pyplot as plt
    fig = plt.figure()
    ax = fig.add_subplot(111)
    xs = [i for i in range(0, Time, 100)]
    ys = get_simulate()
    ax.plot(xs, ys, color='blue')
    # plt.savefig('k=1', dpi=fig.dpi)

# plot; normal; histogram-like
if True:
    import matplotlib.pyplot as plt
    fig = plt.figure()
    ax = fig.add_subplot(111)
    xs = [i for i in range(len(sim))]
    # plt.plot(xs, theor)
    plt.plot(xs, sim)
    plt.show()

# plot; dependency of average x on switch
if False:
    import matplotlib.pyplot as plt
    fig = plt.figure()
    ax = fig.add_subplot(111)

    avs = [get_drift(i/100) for i in range(0, 100, 10)]
    xs = [i for i in range(0, 100, 10)]
    plt.plot(xs, avs)
    plt.show()

