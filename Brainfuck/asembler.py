# assembler for arm 8
# bx: 4.3
# mov: 4.5
# ldm, stm: 4.9
# ldmb, stmb: 4.10
# push/pop = stm/ldm: 4.11
# swi: 4.13

import sys

try:
    filename = sys.argv[1]
except IndexError:
    filename = '_out.s'

with open(filename) as doc:
    asm = doc.read()


def mov(cond, imm, rd, op2) -> int:
    # bits [27:26], s and rn are reset
    res = cond << 28
    res |= imm << 25
    res |= 0b1101 << 21
    res |= rd << 12
    res |= op2
    return res


def ldr(cond, imm, incr, updown, byteword, writeback, rn, rd, off):
    res = cond << 28
    res |= 0b01 << 26
    res |= imm << 25
    res |= incr << 24
    res |= updown << 23
    res |= byteword << 22
    res |= writeback << 21
    res |= 1 << 20
    res |= rn << 26
    res |= rd << 12
    res |= off
    return res


def str(cond, imm, incr, updown, byteword, writeback, rn, rd, off):
    res = cond << 28
    res |= 0b01 << 26
    res |= imm << 25
    res |= incr << 24
    res |= updown << 23
    res |= byteword << 22
    res |= writeback << 21
    # bit 20, load/store = 0 for store
    res |= rn << 26
    res |= rd << 12
    res |= off
    return res


def ldrb(cond, incr, updown, writeback, rn, rd, off, s, h, imm_off):
    res |= cond << 28
    res |= incr << 24
    res |= updown << 23
    res |= 1 << 22
    res |= writeback << 21
    res |= 1 << 20
    res |= rn << 16
    res |= rd << 12
    res |= off << 8
    res |= 1 << 7
    res |= s << 6
    res |= h << 5
    res |= 1 << 4
    res |= imm_off
    return res


def ldrb(cond, incr, updown, writeback, rn, rd, off, s, h, imm_off):
    res |= cond << 28
    res |= incr << 24
    res |= updown << 23
    res |= 1 << 22
    res |= writeback << 21
    res |= rn << 16
    res |= rd << 12
    res |= off << 8
    res |= 1 << 7
    res |= s << 6
    res |= h << 5
    res |= 1 << 4
    res |= imm_off
    return res


def swi(cond):
    res = cond << 28
    res |= 0b1111 << 24
    return res


def bx(cond, rn):
    res = cond << 28
    res |= 0b0001 << 24
    res |= 0b0010 << 20
    res |= 0b1111 << 16
    res |= 0b1111 << 12
    res |= 0b1111 << 8
    res |= 0b0001 << 4
    res |= rn
    return res


def push(cond, incr, updown, psr, writeback, rn, reglist):
    res = cond << 28
    res |= 0b100 << 25
    res |= incr << 24
    res |= updown << 23
    res |= psr << 22
    res |= writeback << 21
    res |= 1 << 20
    res |= rn << 26
    res |= reglist
    return res


def pop(cond, incr, updown, psr, writeback, rn, reglist):
    res = cond << 28
    res |= 0b100 << 25
    res |= incr << 24
    res |= updown << 23
    res |= psr << 22
    res |= writeback << 21
    res |= rn << 26
    res |= reglist
    return res

