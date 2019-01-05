#!/usr/bin/env python3
import sys

cmds = '+-><.,[]'
trans = {
        '+': '\tldrb r1, [r0]\n\tadd r1, r1, #1\n\tstrb r1, [r0]\n',
        '-': '\tldrb r1, [r0]\n\tsub r1, r1, #1\n\tstrb r1, [r0]\n',
        '>': '\tadd r0, r0, #1\n',
        '<': '\tsub r0, r0, #1\n',
        '.': '\tbl puts\n',
        ',': '\tbl gets\n',
        '[': '\tb check_loop_{i}\n\tloop_{i}:\n',
        ']': '\t\tb check_loop_{i}\n\tcheck_loop_{i}:\n\t\tldr r1, [r0]\n\t\tcbnz r1, loop_{i}\n'
        }

try:
    filename = sys.argv[1]
except IndexError:
    filename = 'HelloWorld.b'

with open(filename) as doc:
    src = doc.read()

src = ''.join(i for i in src if i in cmds)

with open('libbf.s') as doc:
    code = doc.read()

code += '.globl main\n'
code += 'main:\n'
code += '\tpush {r4, lr}\n'
code += '\tldr r0, arr_addr\n'

loop_counter = 0

for cmd in src:
    if cmd == '[':
        loop_counter += 1
        code += trans[cmd].format(i=loop_counter)
        continue
    elif cmd == ']':
        loop_counter -= 1
        code += trans[cmd].format(i=loop_counter)
        continue
    code += trans[cmd].replace('\t', '\t' * (loop_counter + 1))

code += '\tpop {r4, lr}\n\tbx lr\n'

with open('_out.s', 'w') as doc:
    doc.write(code)

