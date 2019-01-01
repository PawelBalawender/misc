#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
This module implements a tool that takes as an input
a Brainfuck source code and some short string and outputs
the same, still correct, Brainfuck code, but in the shape of
the particular chars from the short string.
"""
import subprocess
import sys

from PIL import Image, ImageFont
import numpy as np

# setup environment
try:
    input_file = sys.argv[1]
except IndexError:
    print('Usage: BFFormatter.py <input-file>')
    sys.exit(1)

try:
    with open(input_file) as doc:
        source_code = doc.read()
except FileNotFoundError:
    print('No such file:', input_file)
    sys.exit(1)

source_code = ''
text = 'Hello'
font_path = './ttf-bitstream-vera/Vera.ttf'
font_size = 20
output_file = 'bitmap.bmp'

# render the text in wanted font and save it; it's easier this way
# than using PIL
subprocess.call(['convert', '-font', font_path, '-pointsize', str(font_size),
    'label:' + text, output_file])


font = ImageFont.truetype(font_path)
mask = font.getmask(text, font_size)

arr = np.asarray(mask).copy()

print(arr)

res = ''
for row in arr:
    for char in row:
        res += ' ' if char else 'a'
    res += '\n'
print(res)

