#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
This module checks for the symmetry group of a string
Look for symmetry groups in the Internet - those are p1m1, p11m etc.
"""
from math import ceil

# const
file_name = 'slowa.txt'  # file to look for symmetric words in

# letters that have symmetry group p1m1/p11m (in Arial Narrow uppercase)
p1m1 = 'ahimotuvwxy'
p11m = 'bcdehiox'

tab_length = 4  # IDLE-dependent; for me (PyCharm 2017.2.1), Windows 10, it's 4

# load file without newline chars
with open(file_name) as dictionary:
    dictionary = dictionary.read().splitlines()


def is_p1m1(word):
    for i in range(len(word) // 2):  # no checking the center char if it is
        if word[i] != word[-(i + 1)]:  # for [0] it's [-1], not [-0] etc.
            return False
        # filter words with forbidden letters
        elif not set(word).issubset(p1m1):
            return False
    return True


def is_p11m(word: str) -> bool:
    return set(word).issubset(p11m)


def is_p2mm(word: str) -> bool:
    return is_p1m1(word) and is_p11m(word)


# return an int that squared is >= number from argument
def closest_square(n: int) -> int:
    return int(ceil(n**(1/2)))


def print_table(sequence: list):
    sq = closest_square(len(sequence))
    longest = len(max(sequence, key=lambda x: len(x)))

    sequence.sort(key=lambda x: len(x), reverse=True)

    for row in range(sq):
        for col in range(sq):
            index = row*sq + col
            if index == len(sequence):
                print('\n')
                return
            word = sequence[index]

            print('{0:<{x}} '.format(word, x=longest), end='')
        print()

p1m1_words, p11m_words = [], []
for i in dictionary:
    if is_p1m1(i):
        p1m1_words += [i.upper()]
    if is_p11m(i):
        p11m_words += [i.upper()]

p2mm_words = [i for i in p1m1_words if i in p11m_words]

symmetries = {'p1m1': p1m1_words,
              'p11m': p11m_words,
              'p2mm': p2mm_words}

for name, words in symmetries.items():
    print(f'Words with {name} symmetry:')
    print_table(words)
