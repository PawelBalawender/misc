#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
this module takes a match-equation as an input:
---|     ---|     +--+
   |        |     |  |
---|  +  ---|  =  +--+
   |        |        | 
---|     ---|     ---+

and returns every different equation that can
be created from it by swapping a few matches
"""
from typing import List, Tuple
import math

digits = [
        0b1110111,  # 0; every but 3
        0b0100100,  # 1; only 2 and 5
        0b1011101,  # 2; every but 1 and 5
        0b1101101,  # 3; every but 1 and 4
        0b0101110,  # 4; every but 0, 4, 5
        0b1101011,  # 5; every but 2, 4
        0b1111011,  # 6; every but 2
        0b0100101,  # 7; only 0, 2 and 5
        0b1111111,  # 8; every
        0b1101111   # 9; every but 4
        ]


def close_chars(char: bin, chars: List[bin]) -> List[Tuple[bin, int, int]]:
    """
    Find the digits that are similiar to the input
    There can be one match missing or redundant, or one per both of them
    It can also be the original char
    """
    close = []
    for i in chars:
        # bits present in DIGIT, but missing in i
        extra = char & ~i
        # bits missing in DIGIT, but present in i
        missing = i & ~char

        extra = str(bin(extra)).count('1')
        missing = str(bin(missing)).count('1')

        if {extra, missing} in [{0}, {0, 1}, {1, 1}]:
            close += [(digits.index(i), extra, missing)]
    return close


def test_close_chars():
    res = close_chars(digits[0], digits)
    assert res == [(0, 0, 0), (6, 1, 1), (8, 0, 1), (9, 1, 1)]


def parse(equation: str) -> Tuple[int, str, int, str, int]:
    """Parse equation to numbers and signs"""
    a, b, c = tuple(map(int, equation[::2]))
    return a, equation[1], b, equation[3], c


def test_parse():
    assert parse('2+3=9') == (2, '+', 3, '=', 9)


def check(equation: str) -> bool:
    """Check if the equation is correct"""
    if '=' not in equation:
        return False

    equality = eval(equation.replace('=', '=='))
    return equality


def test_check():
    assert not check('2+3=9')
    assert not check('2+3+3')
    assert not check('2+3-3')
    assert not check('2-3-3')
    assert not check('1+1=1')
    assert check('2+3=5')
    assert check('5=2+3')
    assert check('9-9=0')
    assert check('3=7-4')
    assert check('3=3=3')


def get_possible(parsed):
    """
    Take the original equation, parsed, and return all the possible
    equations that we can crete from it (mainly incorrect!)
    """

    nums = parsed[::2]
    nums = [close_chars(digits[i], digits) for i in nums]
    
    sgns_possibilities = {
            '+': [('+', 0, 0), ('-', 1, 0), ('=', 1, 1)],
            '-': [('-', 0, 0), ('+', 0, 1), ('=', 0, 1)],
            '=': [('=', 0, 0), ('+', 1, 1), ('-', 1, 0)]
            }

    sgns = parsed[1::2]
    sgns = [sgns_possibilities[i] for i in sgns]

    possible = nums
    possible.insert(1, sgns[0])
    possible.insert(3, sgns[1])
    
    return possible


def test_get_possible():
    eq = '2+3=9'
    okay = [[(2, 0, 0), (3, 1, 1)], 
            [('+', 0, 0), ('-', 1, 0), ('=', 1, 1)], 
            [(2, 1, 1), (3, 0, 0), (5, 1, 1), (9, 0, 1)], 
            [('=', 0, 0), ('+', 1, 1), ('-', 1, 0)], 
            [(0, 1, 1), (3, 1, 0), (5, 1, 0), (6, 1, 1), (8, 0, 1), (9, 0, 0)]]
    assert get_possible(parse(eq)) == okay


def get_all(ready: str, possibilities, extra: int=0, missing: int=0) -> List[str]:
    """
    Get all correct answers
    
    Since we can only move one match, it means, that we have to substract it
    somewhere and then place somewhere else - so extra has to be equal to
    missing (extra == missing == 1). If either of them exceeds 1, it's
    impossible to make it by only moving one match - this means, that it's
    incorrect and to rewind and find another options is what we do

    :param ready: already checked part of the equation
    :param possibilities: part of the equation that we aren't certain about
    :param extra: counter of matches that we already have to add somewhere
    :param missing: counter of matches that we already miss somewhere
    :return: list of all equations that are correct
    """
    # print('invoked!', f'left: {len(possibilities)} chars')
    if not possibilities:
        return [ready] if check(ready) and {extra, missing} in [{0}, {1, 1}] else []

    # print(possibilities)
    correct = []
    for i in possibilities[0]:
        new_extra = extra + i[1]
        new_missing = missing + i[2]
        if new_extra > 1 or new_missing > 1:
            continue

        correct += get_all(ready + str(i[0]), possibilities[1:], new_extra,new_missing)
    return correct


def test_get_all():
    assert get_all('2+3=9', []) == []
    assert get_all('2+3=5', []) == ['2+3=5']
    assert get_all('2+3=', [[(2, 0, 0), (5, 1, 1), (6, 1, 0)]]) == ['2+3=5']
    assert get_all('2+', [
        [(3, 1, 1), (5, 1, 1)],
        [('+', 1, 1), ('=', 0, 0), ('-', 0, 1)],
        [(5, 1, 1), (7, 0, 0)]
        ]) == ['2+5=7']


def run_tests():
    test_close_chars()
    test_parse()
    test_check()
    test_get_possible()
    test_get_all()

if __name__ ==  '__main__':
    run_tests()

    # equation = input('> ')
    equation = '3+3=6'

    parsed = parse(equation)
    possible = get_possible(parsed)

    correct = get_all('', possible)

    print(equation)
    print(correct)
