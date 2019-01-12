#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
This module is capable of transforming arabic number to roman
and in reverse; it takes advantage of the roman apostrophe-system
to operate on big numbers
"""
from math import ceil

# user data
# better not to make this range too large
# time complexity is O(too much)
start = 0  # beginning of the number range to search
end = 10**4  # and it's end

# const
values = {'I': 1,
          'V': 5,
          'X': 10,
          'L': 50,
          'C': 100,
          'D': 500,
          'M': 1000}
values_rev = {val: key for key, val in values.items()}


def is_bracket(char: str) -> bool:
    return char in ['(', '|', ')']


def is_apostrophe(sign: str) -> bool:
    return set(sign).issubset({'(', '|', ')'})


def apostrophe_value(apostrophe: str) -> int:
    n = apostrophe.count(')')
    number = 10 ** (n + 2)  # (|) 1000, ((|)) 10000

    if apostrophe.startswith('('):  # (|) 1000
        return number
    else:  # |) 500, |)) 5000
        return number // 2


def integer_from_roman_sign(sign: str) -> int:
    if is_apostrophe(sign):
        return apostrophe_value(sign)
    else:
        return values[sign]


def parse_roman(roman: str) -> list:
    roman = roman.upper()

    # parse number to apostrophes and 'alphabetical' parts
    sequence = []
    for i, j in enumerate(roman):
        if i == 0:  # pass, because there's no previous element
            sequence += [j]
            continue

        # append in one part if it's apostrophe, otherwise separately
        if is_bracket(j) == is_bracket(roman[i - 1]) is True:
            sequence[-1] += j
        else:
            sequence += [j]

    # check sequence one more time and divide concatenated apostrophes
    # option 1: ')(' boundary, eg. (|)((|))
    for i, j in enumerate(sequence):
        if j.count('|') > 1:
            divided = j.split(')(')
            for k in range(len(divided) - 1):  # -1 not to deal with last el.
                divided[k] += ')'  # restore lost delimiters
                divided[k + 1] = '(' + divided[k + 1]

            # insert distinguished apostrophes on their place
            sequence.pop(i)
            # divided reversed, because it'll reversed again by
            # inserting
            [sequence.insert(i, sign) for sign in divided[::-1]]

    # option 2: ')|' boundary, eg. ((|))|)
    for i, j in enumerate(sequence):
        if j.count('|') > 1:
            divided = j.split(')|')
            for k in range(len(divided) - 1):  # -1 not to deal with last el.
                divided[k] += ')'  # restore lost delimiters
                divided[k + 1] = '|' + divided[k + 1]

            # insert distinguished apostrophes on their place
            sequence.pop(i)
            # divided reversed, because it'll reversed again by
            # inserting
            [sequence.insert(i, sign) for sign in divided[::-1]]

    return sequence


def to_arabic(roman: str) -> int:
    sequence = parse_roman(roman)

    # convert roman numbers to integers one by one
    sequence = [integer_from_roman_sign(i) for i in sequence]

    # sum up the numbers, keeping in mind that some numbers are
    # represented as a difference
    number = 0
    for i, j in enumerate(sequence):
        if i == len(sequence) - 1:
            number += j  # pass, because there's no next element
            continue

        next_val = sequence[i + 1]

        if next_val > j:
            number -= j
        else:
            number += j
    return number


def roman_sign_from_integer(n: int) -> str:
    if n <= 1000:
        return values_rev[n]

    # else: create and return an apostrophe
    magnitude = len(str(n)) - 1  # could use math.log10() instead
    a = magnitude - 1  # amount of brackets needed

    if n == 10 ** (magnitude + 1):
        return '(' * a + '|' + ')' * a
    else:
        return '|' + ')' * a


def parse_to_roman(n: int) -> str:
    number = str(n)

    sequence = []
    for i in range(len(number) - 1, -1, -1):  # end := -1, because we want 0 too
        diff = n // (10 ** i)
        n -= diff * (10 ** i)

        if diff % 10 == 10 - 1:  # 9
            sequence += [10 ** i]
            sequence += [10 * (10 ** i)]
        elif diff // 5:  # 5 to 8
            sequence += [5 * (10 ** i)]
            sequence += [(10 ** i)] * (diff % 5)
        elif diff % 5 == 5 - 1:  # 4
            sequence += [10 ** i]
            sequence += [5 * (10 ** i)]
        else:  # 0 to 3
            sequence += [(10 ** i)] * diff
    return sequence


def to_roman(n: int) -> str:
    sequence = parse_to_roman(n)
    return ''.join(roman_sign_from_integer(i) for i in sequence)


# return an int that squared is >= number from argument
def closest_square(n: int) -> int:
    return int(ceil(n**(1/2)))


def print_table(sequence: list):
    sq = closest_square(len(sequence))
    longest = len(max(sequence, key=lambda x: len(x)))

    for row in range(sq):
        for col in range(sq):
            index = row*sq + col
            if index == len(sequence):
                print('\n')
                return
            word = sequence[index]

            print('{0:<{x}} '.format(word, x=longest), end='')
        print()


def test() -> bool:
    test_case = {
        'i': 1,
        'IV': 4,
        'VIiI': 8,
        'IX': 9,
        'Xxx': 30,
        'XL': 40,
        'XLIX': 49,
        'CXix': 119,
        'cd': 400,
        '|)cXciv': 694,
        '(|)': 1000,
        '(|)|)': 1500,
        '((|))(((|)))': 90000,
        '((|))(((|)))m((|))|)cxi': 99611
    }

    for i, j in test_case.items():
        if to_arabic(i) != j:
            print(i)

        if to_roman(j) != i:
            print(j)


numbers = []
for i in range(start, end):
    if set(str(i)).issubset({'0', '3', '8'}) and i:
        roman = to_roman(i)
        if set(roman).issubset({'I', 'X', 'C', 'D', '(', '|', ')'}):
            numbers += [i]

print(len(numbers), f'numbers in range {start} to {end}:\n')

# print_table([str(i) for i in numbers])

roman_numbers = [to_roman(i) for i in numbers]
print_table(sorted(roman_numbers, key=lambda x: to_arabic(x)))
