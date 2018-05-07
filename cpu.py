#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
This module fools with some low-level tasks such as
using processor's instructions for your purposes etc.

todo: implement Brainfuck interpreter on it
"""
import threading
import sys

import matplotlib.pyplot as plt


def PWM(config):
    plt.ion()

    fig = plt.figure()
    ax = fig.add_subplot(111)

    ax.set_xlim([0, 1])
    ax.set_ylim([0, 1])

    circle = plt.Circle((0.5, 0.5), 0.05, color='blue')
    ax.add_artist(circle)

    while True:
        period = config[0]
        if period:
            duty_cycle = config[1]
            circle.set_color('blue')
            plt.pause(period*duty_cycle)
            circle.set_color('white')
            plt.pause(period*(1-duty_cycle))

config = [0, .2]
t = threading.Thread(target=PWM, args=(config,))
t.start()


class Cpu:
    CPU_HALT = 2
    CPU_EZ = 1

    def __init__(self, code):
        self.operations = {
            'ADD': self.add,
            'PUSH': self.push,
            'POP': self.pop,
            'HALT': self.halt,
            'EZ': self.ez,
            'CB': self.cb,
            'PUT': self.put,
            'SWP': self.swp,
            'MOV': self.mov,
        }

        self.acc = 0
        self.counter = 0
        self.status = 0
        self.pc = -1
        self.code = code

        self.mem = [0 for _ in range(1024)]
        self.sp = len(self.mem)

    def add(self, val):
        self.acc += int(val)

    def push(self, val):
        self.sp -= 1
        self.mem[self.sp] = int(val)

    def pop(self):
        self.acc = self.mem[self.sp]
        self.sp += 1

    def halt(self):
        self.status |= self.CPU_HALT

    def ez(self):
        if not self.acc:
            self.status |= self.CPU_EZ
        else:
            self.status &= ~self.CPU_EZ

    def cb(self, val):
        if (self.status & self.CPU_EZ) == self.CPU_EZ:
            self.pc = int(val)-1 # -1 only because it's incremented before execution

    def put(self):
        print(chr(self.acc), end='', file=sys.stderr)

    def swp(self):
        acc = self.acc
        self.acc = self.counter
        self.counter = acc

    def mov(self, src, dest):
        if dest == 'PERIOD':
            config[0] = float(src)
        elif dest == 'DUTY':
            config[1] = float(src)

    def run(self):
        while (self.status & self.CPU_HALT) != self.CPU_HALT:
            self.pc += 1
            print(self)
            self.execute(self.code[self.pc])

    def execute(self, cmd):
        func, *item = cmd.split()
        func = self.operations[func]
        func(*item)

    def __str__(self):
        return "STACK:"+"".join(map(lambda x: "\n "+str(x), self.mem[self.sp:]))+" <-\nACC: "+str(self.acc)+"\n"+"COUNTER: "+str(self.counter)+"\n"+"SP: "+str(self.sp)+"\n"\
               +"STATUS: "+bin(self.status)[2:].zfill(8)+"\n"+"PC: "+str(self.pc)+" [%s]"%self.code[self.pc]+"\n"


# def test_add():
#     cpu = Cpu()
#
#     acc = cpu.acc
#     cpu.add(7)
#     assert acc + 7 == cpu.acc
#
#     acc = cpu.acc
#     cpu.add(-19)
#     assert acc - 19 == cpu.acc

# test_add()


# cpu = Cpu()
# cpu.execute(["ADD 1"]) # expected: ACC is incremented by 1
# print(cpu)
# cpu.execute(["PUSH 1"]) # expected: *(SP)==1, then SP is decremented by 1
# print(cpu)
# cpu.execute(["POP"])
# print(cpu)
# cpu.execute(["EZ"]) # expected: STATUS |= EZ if ACC equals zero, STATUS &= ~EZ
# cpu.execute(["CB 1"]) # expected: set PC to 1 if STATUS&EZ == EZ, do nothing otherwise

#cpu = Cpu(["ADD 1", "EZ", "ADD -1", "EZ", "ADD 3", "EZ", "HALT"])
#cpu = Cpu(["PUSH %i" % ord(i) for i in "Hello"][::-1]+["ADD 5", "SWP", "POP", "PUT", "SWP", "ADD -1", "EZ", "CB 18", "SWP", "PUSH 0", "POP", "EZ", "CB 7", "HALT"])

cpu = Cpu(["MOV .001 PERIOD", "MOV .5 DUTY" , "HALT"])

cpu.run()

t.join()
