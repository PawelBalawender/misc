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

with open('HelloWorld.b') as doc:
    src = doc.read()

src = ''.join(i for i in src if i in cmds)

with open('libbf.s') as doc:
    code = doc.read()

code += '.globl main\n'
code += 'main:\n'
code += 'push {r4, lr}\n'
code += 'ldr r0, arr_addr\n'

loop_counter = 0
loop_stack = []

for cmd in src:
    if cmd == '[':
        loop_counter += 1
        code += 'b _loop_cond_{}\n'.format(loop_counter)
        code += '_loop_{}:\n'.format(loop_counter)
        loop_stack.append(loop_counter)
        continue
    elif cmd ==']':
        ending = loop_stack.pop()
        code += 'b _loop_cond_{}\n'.format(ending)
        code += '_loop_cond_{}:\n'.format(ending)
        code += 'ldr r1, [r0]\n'
        code += 'cmp r1, #0\n'
        code += 'bne _loop_{}\n'.format(ending)
        continue

    code += trans[cmd]

code += 'pop {r4, lr}\n'
code += 'bx lr\n'

with open('_out.s', 'w') as doc:
    doc.write(code)

