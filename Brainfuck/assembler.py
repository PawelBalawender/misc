# assembler for arm v8A, A32 version

import sys

sp = 13
lr = 14
pc = 15

try:
    filename = sys.argv[1]
except IndexError:
    filename = '_out.s'

with open(filename) as doc:
    asm = doc.read()


def reglist_to_bin(regs*):
    res = 0
    for i in regs:
        regs |= 1 << i
    return regs

