#!/usr/bin/env python3

cmds = '+-><.,[]'
trans = {
        '+': 'ldrb r1, [r0]\nadd r1, r1, #1\nstrb r1, [r0]\n',
        '-': 'ldrb r1, [r0]\nsub r1, r1, #1\nstrb r1, [r0]\n',
        '>': 'add r0, r0, #1\n',
        '<': 'sub r0, r0, #1\n',
        '.': 'bl puts\n',
        ',': 'bl gets\n',
        }

with open('naive_hello.b') as doc:
    src = doc.read()

src = ''.join(i for i in src if i in cmds)

with open('libbf.s') as doc:
    code = doc.read()

code += '.globl main\n'
code += 'main:\n'
code += 'mov r0, arr_addr\n'

for cmd in src:
    if cmd in '[]':
        continue

    code += trans[cmd]

with open('_out.s', 'w') as doc:
    doc.write(code)

