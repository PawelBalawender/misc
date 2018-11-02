#!usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
This module is used to perform a simulation described in detail on
a paper sheet; there's no point in describing the goal there, but
in general, it simulates the motion of a cell in a periodic potential
well; look: Brown's motions, Brown's engine
"""
from random import uniform
from math import sqrt
from time import time
from decimal import Decimal
import sys

import numpy as np


class NullOut():
    def __init__(self):
        self.old_out = sys.stdout

    def __enter__(self):
        sys.stdout = self

    def __exit__(self, exc_type, exc_val, exc_tb):
        sys.stdout = self.old_out

    def write(self, _):
        pass


class Constant(object):
    def __setattr__(self, key, value):
        if hasattr(self, key):
            raise TypeError('cannot redeclare a constant')
        super().__setattr__(key, value)

const = Constant()

const.k = 1.38064852 / (10 ** 23)  # Boltzmann's constant

const.gamma = 1  # coefficient of friction of the particle's environment

# formula: kT = 0.2V_max (0.2 is custom, it is not the asymmetry factor)
const.V_max = 1  # potential well's boundaries' potential
const.T = const.V_max * 0.2 / const.k  # temperature of environment

# intensity of noise; formula: alpha = sqrt(2kT/gamma)
const.alpha = sqrt(2 * const.k * const.T / const.gamma)

const.L = 1  # length of the potential well
const.A = 0.2  # well's asymmetry factor

# custom
# delta t; change of the time
# it affects the change in particle's position and thus should be
# small enough to [delta_x <<< const.L]
const.delta_t = 0.001
# the following is used when calculating the random factor, W(delta_t)
const.sqrt_three_delta_t = sqrt(3 * const.delta_t)
# the following is used when calculating the particle's move
const.delta_t_on_gamma = const.delta_t / const.gamma

const.parts = 20  # number of bins on the outcome histogram
const.step = const.L / const.parts  # size of bins of the hist


def simulate(sim_time: int=0, x: float=0, potential: float=0,
             force: float=0) -> np.ndarray:
    # to both restrain the amount of data in the RAM
    # and maintain the performance,
    # the function uses temporary Python native list, which can be updated
    # faster than ndarray, and then, after a few iterations,
    # transfer records to ndarray
    np_array = np.array(x)  # the main array; initial value: first x
    _positions = []  # the temporary array

    # i's purpose: giving a feedback to the user
    for i in range(10, 110, 10):
        a = time()
        # j's purpose: clear the temporary array and transfer the data to numpy
        for j in range(10):
            # the main array
            # // 100, because 'phase' loop will be invoked i=10*j=10 = 100 times
            # floor division to ensure that it is integer
            for phase in range(sim_time // 100):
                # move the particle
                # formula: delta_x = force/gamma * delta_t + alpha * W(delta_t)
                # w is W(delta_t), the random factor
                w = uniform(-1, 1) * const.sqrt_three_delta_t

                # we can change [force / gamma * delta_t]
                # to [force * (delta_t / gamma)] and make
                # the second factor a constant and so we do
                delta_x = force * const.delta_t_on_gamma + const.alpha * w
                x += delta_x

                old_potential = potential
                # map the x to the (-1, 1) area
                x -= int(x)
                # map the x to periodical cell
                if x <= -0.8:  # x <= aL - L; will be on the right hill
                    x += 1
                    potential = 5 * x
                elif x < 0:  # x is already on the left hill
                    potential = -1.25 * x
                elif x > 0.2:  # x > aL; will be on the left hill
                    x -= 1
                    potential = -1.25 * x
                elif x > 0:  # x is already on the right hill
                    potential = 5 * x

                delta_potential = potential - old_potential

                # calculate the force influencing the particle by formula:
                # force = -(dV) / (dx)
                force = -delta_potential / delta_x

                _positions += [x]
            np_array = np.append(np_array, _positions)
            _positions.clear()
        print(f'progress: {i}%, iteration time: {time() - a}')
    return np_array


def main(mode: int, sim_time: int, repeats: int=2):
    # modes: 0: no plot, 1: save plot, 2: show plot, 3: save & show
    # below is just auxiliary; aL - L = -0.8
    left_border = const.A * const.L - const.L

    if mode:
        import matplotlib.pyplot as plt

        ax = plt.subplot(111)

        # 0.2 below is custom
        ticks = np.arange(left_border, const.A + const.step, 0.2)
        # round, because 0 is represented as -2.2244...e-16
        ticks = [round(i, 1) for i in ticks]
        ax.set_xticks(ticks)

        labels = [str(i) for i in ticks]
        labels[0] = '(a - 1)L'
        labels[-1] = 'aL'
        ax.set_xticklabels(labels)

    for i in range(repeats):
        a = time()
        positions = simulate(sim_time)

        bins = np.arange(left_border,
                         const.A + const.step,  # + to count the last element
                         const.step)

        if mode:
            n, _, patches = ax.hist(positions, bins, rwidth=0.95, normed=1)

            # normalize bins, so their heights sum up to 1
            n = sum(n)
            highest = 0
            for patch in patches:
                h = patch.get_height() / n
                patch.set_height(h)
                highest = max(highest, h)

            ax.set_xlim([left_border, const.A])
            ax.set_ylim([0, highest * 1.1])  # * 1.1 to adjust it a little

            text = f'time: {sim_time}\nfriction: {const.gamma}'
            ax.text(0, highest * 0.9, text)  # 0 and 0.9 are custom

            if mode % 2:
                file_name = f'{i}.png'
                plt.savefig(file_name)
            if mode // 2:
                plt.show()
            plt.cla()
        print('time:', time() - a)

if __name__ == '__main__':
    # with NullOut():
    main(mode=1, sim_time=10**8, repeats=3)
