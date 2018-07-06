#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
This module implements an interpreter for the Deadfish esolang, but with loops.

Deadfish, in original, has one single register, on which you can perform addition, substraction and squaring. You can also output it, but not input. 

In this implementation loops were added. There's only one type of a loop, made by:
    if COUNTER > 0: jump_back JUMP_LENGTH operations
So there are two more registers - CC, which provides us a counter, and JX, which holds the length (in bytes) by which we jump back in the code.
You can increment CC by '*' operation, increment JX by '^' operation, decrement the latter by '_'. Decrementing the CC occurs only while jumping.

You can also add comments - every char that's not a command will be discarded anyway, but if you want to write some 'dangerous' comments, such as 'if a < b' (you use '<' sign there, which is the conditional jump operator; it isn't neutral), you can make an inline comment by '|' sign.
The whole part from '|' to newline will be discarded.

'!' sign stands for a simple debugging tool, which outputs values of registers.

Value in the register zeroes when it's equal to either -1 or 256
"""

code = """
^^^^^^^^    | init: set jump to -8
iiiiiiii
s
iiiiiiii
o           | 8^2 + 8 = 72, H

***
iiiiiiii<   | +4*8 = 72+32 = 104
ddd         | 101, e
o

iiiiiii
oo          | 108, l

iii
o           | 111, o 111-44 = 67

*******
dddddddd<
ddd         | 111 - 8*8 - 3 = 111 - 67 = 44, ","
o

dddddd
dddddd
o           | 32, " "

            | "World!": 87, 111, 114, 108, 100, 33

******
iiiiiiii<   | +56, 88
d
o           | 87, W

**
iiiiiiii<   | +24
o           | 111, o

iii
o           | 114, r

dddddd      | 108, l
o

dddddddd    | 100, d
o

*******
dddddddd<   | -8*8 = 36
ddd
o           | 33, "!"
"""

ACC = 0  # accumulator
JX = 0  # jump length register
CC = 0  # counter
EP = 0  # execution pointer

def incr():
    global ACC
    ACC += 1

def decr():
    global ACC
    ACC -= 1

def sq(): 
    global ACC
    ACC **= 2

def out():
    print(chr(ACC), end='')

def jx_add():
    global JX
    JX += 1

def jx_sub():
    global JX
    JX -= 1

def cc_add():
    global CC
    CC += 1

def jump():
    global EP, JX, CC
    if CC:
        EP -= JX
        CC -= 1
        EP -= 1  # incr at the end of the main loop would make a mess

def dbg():
    print(f'ACC: {ACC} JX: {JX} CC: {CC}')

CMDS = {
        'i': incr,
        'd': decr,
        's': sq,
        'o': out,
        '^': jx_add,
        '_': jx_sub,
        '*': cc_add,
        '<': jump,
        '!': dbg,
        }


def prepare(code: str) -> str:
    """Take code and output just the operations that it contains"""
    c = 0
    while c < len(code):
        if code[c] == '|':
            code = code[:c] + code[code.index('\n', c):]

        c += 1

    return ''.join(i for i in code if i in CMDS.keys())


def test_prepare():
    code = """
    iii  | add 3
    o
    """
    assert prepare(code) == 'iiio'
test_prepare()


code = prepare(code)
while EP < len(code):
    cmd = code[EP]
    CMDS[cmd]()

    if ACC in {-1, 256}:
        ACC = 0

    EP += 1

print()

