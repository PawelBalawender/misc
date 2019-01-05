#!/usr/bin/env python3

def f(cond, x, i, opcode, s, rn, rd, op2):
    return hex(op2 + (rd << 12) + (rn << 16) + (s << 20) + (opcode << 21) + (i << 25) + (x << 26) + (cond << 28))

always = 0b1110
x = 0
op2_is_imm = 1
mov = 0b1101
dont_alter_cmps = 0
rn = 0
rd = 1
op2 = 1

print(f(always, x, op2_is_imm, mov, dont_alter_cmps, rn, rd, op2))

