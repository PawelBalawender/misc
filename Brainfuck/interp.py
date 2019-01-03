#!/usr/bin/env python3
import sys

# define the Brainfuck language
cmds = {
        '+': 'ARR[P] += 1',
        '-': 'ARR[P] -= 1',
        '>': 'P += 1',
        '<': 'P -= 1',
        '[': 'while ARR[P]:',
        ']': '',
        '.': 'print(chr(ARR[P]), end="")',
        ',': 'ARR[P] = int(input())',
        }

# read Brainfuck source code, then trim whitespaces and comments
try:
    inp_file = sys.argv[1]
except IndexError:
    print('Usage: interp.py <source-file-path>')
    sys.exit(1)

try:
    with open(inp_file, 'r') as doc:
        prog = doc.read()
except FileNotFoundError:
    print('No such file:', inp_file)
    sys.exit(1)

prog = [i for i in prog if i in cmds.keys()]

# convert to Python
indent = 0
python_prog = """
ARR = [0 for i in range(30000)]
ACC = 0
P = 0
"""

for cmd in prog:
    python_prog += '\t' * indent
    python_prog += cmds[cmd] + '\n'
    if cmd == '[':
        indent += 1
    elif cmd == ']':
        indent -= 1

# execute the output code
try:
    exec(python_prog)
except IndexError:
    print('Buffer overflow')
    sys.exit(1)

