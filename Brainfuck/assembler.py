#!/usr/bin/env python3
# assembler for arm v8A, A32 version

import sys


SP = 13
LR = 14
PC = 15
ALWAYS = 0b1110


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

    def ldmia(cond, w, rn, reglist):
        encoded = (cond << 28) | (0x8 << 24) | (0b10 << 22) | (w << 21)
        encoded |= (1 << 20) | (rn << 16) | reglist
        return encoded

    def stmdb(cond, w, rn, reglist):
        return (cond << 28) | (0x9 << 24) | (w << 21) | (rn << 16) | reglist


class Abstractions:
    def b(addr):
        return LowLevel.b(ALWAYS, addr)

    def bl(addr):
        return LowLevel.bl(ALWAYS, addr)

    def bx(reg):
        return LowLevel.bx(ALWAYS, reg)

    def swi(comment):
        return LowLevel.svc(ALWAYS, comment)

    def mov_reg(dest, source):
        imm12 = source << 7
        return LowLevel.mov(ALWAYS, i=0, s=0, rd=dest, imm12=imm12)

    def mov_imm(dest, imm):
        # todo: handle numbers bigger than 31
        imm12 = imm
        return LowLevel.mov(ALWAYS, i=1, s=0, rd=dest, imm12=imm12)

    def ldr_reg(dest_reg, base_reg, off):
        args = {
                'cond': ALWAYS,
                'i': 0,
                'p': 1,
                'u': (off > 0),
                'size': 0,
                'w': 0,
                'ls': 1,
                'rn': base_reg,
                'rt': dest_reg,
                'imm12': abs(off),
                }
        return LowLevel.load_store(**args)

    def ldr_imm(dest_reg, addr):
        args = {
                'cond': ALWAYS,
                'i': 1,
                'p': 1,
                'u': 1,
                'size': 1,
                'w': 0,
                'ls': 1,
                'rn': 0,
                'rt': dest_reg,
                'imm12': addr,
                }
        return LowLevel.load_store(**args)

    def ldrb_reg(dest_reg, addr_reg):
        args = {
                'cond': ALWAYS,
                'i': 0,
                'p': 1,
                'u': 1,
                'size': 0,
                'w': 0,
                'ls': 1,
                'rn': addr_reg,
                'rt': dest_reg,
                'imm12': 0,
                }
        return LowLevel.load_store(**args)

    def ldrb_imm(dest_reg, addr):
        args = {
                'cond': ALWAYS,
                'i': 1,
                'p': 1,
                'u': 1,
                'size': 0,
                'w': 0,
                'ls': 1,
                'rn': 0,
                'rt': dest_reg,
                'imm12': addr,
                }
        return LowLevel.load_store(**args)

    def str_reg(sorce_reg, addr_reg):
        args = {
                'cond': ALWAYS,
                'i': 0,
                'p': 1,
                'u': 1,
                'size': 1,
                'w': 0,
                'ls': 0,
                'rn': addr_reg,
                'rt': source_reg,
                'imm12': 0,
                }
        return LowLevel.load_store(**args)

    def strb_reg(sorce_reg, addr_reg):
        args = {
                'cond': ALWAYS,
                'i': 0,
                'p': 1,
                'u': 1,
                'size': 0,
                'w': 0,
                'ls': 0,
                'rn': addr_reg,
                'rt': source_reg,
                'imm12': 0,
                }
        return LowLevel.load_store(**args)

    def pop(*regs):
        reglist = 0
        for i in regs: reglist |= (1 << i)
        return LowLevel.ldmia(ALWAYS, w=1, rn=SP, reglist=reglist)

    def push(*regs):
        reglist = 0
        for i in regs: reglist |= (1 << i)
        return LowLevel.stmdb(ALWAYS, w=1, rn=SP, reglist=reglist)


if __name__ == '__main__':
    try:
        filename = sys.argv[1]
    except IndexError:
        filename = 'hello.s'

    with open(filename) as doc:
        asm = doc.read()

    # 1. resolve references, .data section, .set and .word directives

    a = Abstractions
    x = [
            a.ldr_reg(1, 15, 20),
            a.push(7, LR),
            a.mov_imm(0, 1),
            a.mov_imm(2, 15),
            a.mov_imm(7, 4),
            a.swi(0),
            a.mov_imm(0, 0),
            a.pop(7, LR),
            a.bx(LR)
            ]
    for i in x:
        print(hex(i))

