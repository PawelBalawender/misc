#!/usr/bin/env python3
# assembler for arm v8A, A32 version

import sys

sp = 13
lr = 14
pc = 15


class LowLevel:
    def b(cond, imm24):
        return (cond << 28) | (0xA << 24) | imm24

    def bl(cond, imm24):
        return (cond << 28) | (0xB << 24) | imm24

    def bx(cond, rm):
        return (cond << 28) | (0x12FFF1 << 4) | rm

    def svc(cond, imm24):
        return (cond << 28) | (0xF << 24) | imm24

    def mov(cond, i, s, rd, imm12):
        encoded = (cond << 28) | (i << 25) | (0xD << 21) | (s << 20)
        encoded |= (rd << 12) | imm12
        return encoded

    def load_store(cond, i, p, u, size, w, ls, rn, rt, imm12):
        encoded = (cond << 28) | (1 << 26) | (i << 25) | (p << 24)
        encoded |= (u << 23) | (size << 22) | (w << 21) | (ls << 20)
        encoded |= (rn << 16) | (rt << 12) | imm12
        return encoded


ALWAYS = 0b1110


class Abstractions:
    def b(addr):
        return LowLevel.b(ALWAYS, addr)

    def bl(addr):
        return LowLevel.bl(ALWAYS, addr)

    def bx(reg):
        return LowLevel.bl(ALWAYS, reg)

    def swi(comment):
        return LowLevel.svc(ALWAYS, comment)

    def mov_reg(dest, source):
        return LowLevel.mov()

def imm12(imm5, typ, rm):
    return (imm5 << 7) | (typ << 5) | rm


def reglist_to_bin(*regs):
    res = 0
    for i in regs:
        regs |= 1 << i
    return regs

try:
    filename = sys.argv[1]
except IndexError:
    filename = 'hello.s'

ops = {
        'b': b,
        'bx': bx,
        'bl': bl,
        'swi': swi,
        'mov': mov,
        'ldr': ldr,
        'str': strx,
        'ldrb': ldrb,
        'strb': strb,
        'push': push,
        'pop': pop,
        }

with open(filename) as doc:
    while not 'main:' in doc.readline():
        continue

    for line in doc.readlines():
        tokens = line.split()
        tokens = [''.join(c for c in x if c.isalpha() or c.isdigit()) for x in tokens]
        print(ops[tokens[0]](tokens[1:]))

