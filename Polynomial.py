#!/usr/bin/env python
# -*- coding: UTF-8 -*-
"""
This module implements an interpreter for the Polynomial esoteric language
"""
from collections import namedtuple
from typing import List, Tuple, Set

import numpy as np

cat = 'f(x) = x^10 - 4827056x^9 + 1192223600x^8 - 8577438158x^7 + 958436165464x^6 - 4037071023854x^5 + 141614997956730x^4 - 365830453724082x^3 + 5225367261446055x^2 - 9213984708801250x + 21911510628393750'

Operation = namedtuple('Operation', ['operation', 'operand'])


def parse_monomial(monomial: str) -> Tuple[int, int]:
    """
    Take a single monomial-string and extract its coefficient and power of x

    Example:
    '+x^10'         ->  1,          10
    '-4827056x^9'   ->  -4827056,   9
    """
    assert monomial[0] in ['+', '-']

    if 'x' in monomial:  # case: +x^n
        ind = monomial.index('x')
        if not monomial[ind-1].isdigit():
            monomial = monomial[0] + '1' + monomial[1:]

    if 'x^' in monomial:
        coefficient_end = monomial.index('x^')
        power_beginning = coefficient_end + 2
        power = int(monomial[power_beginning:])
    elif 'x' in monomial and '^' not in monomial:
        coefficient_end = monomial.index('x')
        power = 1
    elif 'x' not in monomial and '^' not in monomial:
        coefficient_end = len(monomial)
        power = 0
    else:
        raise Exception('Number in a wrong format')

    coefficient = int(monomial[:coefficient_end])
    return coefficient, power


def parse_polynomial(polynomial: str) -> List[Tuple[int, int]]:
    """
    Take the whole Polynomial program as a string and perform parsing on its
    each element. Return a sequence of [(coefficient, power of x)];
    powers will be in descending order, but some of them can be missing;

    Example:
    f(x) = x^4 - 3x^2 + 2
    -> [(1, 4), (-3, 2), (2, 0)]
    """
    polynomial = polynomial.split()[2:]
    # first element can go without sign, if it's a +
    if polynomial[0] != '-':
        polynomial = ['+'] + polynomial

    # merge monomial with its sign
    merged = []
    for i in range(0, len(polynomial), 2):  # len(pol) always is divisible by 0
        merged += [polynomial[i] + polynomial[i+1]]
    return [parse_monomial(i) for i in merged]


def get_coefficients(_pairs: List[tuple]) -> List[int]:
    """
    Take a sequence like [(coefficient, power of x)] and add zeros where are
    they missing
    """
    _coefficients = []
    power_counter = _pairs[0][1]  # highest coefficient
    for pair in _pairs:
        # add missing coefficients
        while pair[1] != power_counter:
            _coefficients += [0]
            power_counter -= 1

        # add actual coefficients
        _coefficients += [pair[0]]
        power_counter -= 1
    return _coefficients

# todo: parser could use yielding and save some memory
pairs = parse_polynomial(cat)  # ok
coefficients = get_coefficients(pairs)  # ok
roots = np.roots(coefficients)  # ok

# eliminate almost-zeros and complex conjugates
roots.real[abs(roots.real) < 1e-11] = 0
roots.imag[abs(roots.imag) < 1e-11] = 0
roots = roots[roots.imag >= 0]


def generate_primes():
    _primes = {2}
    yield 2
    current = 3
    while True:
        if all(current % i != 0 for i in _primes):
            # nothing has divided our prime - it is a prime
            _primes.add(current)
            yield current
        current += 2  # omit even numbers

gen = generate_primes()
primes = [next(gen) for _ in range(len(coefficients))]


def get_actual_zeros(_roots: np.ndarray) -> list:
    """
    Essential thing about parsing Polynomial code - take original polynomial's
    zeros and find the actual operations that they correspond to
    """
    found: Set[np.complex128] = set()
    operations: List[Tuple[np.complex128, int, int]] = []

    for prime in primes:
        for root in _roots:
            if root in found:
                continue

            if root.imag:  # a+bi -> a + (p^b)i
                exp = np.log(root.imag) / np.log(prime)
            else:  # a -> p^a
                exp = np.log(root.real) / np.log(prime)

            # if exp is int or almost-int - we found it!
            # int_exp is our actual zero - so the operation
            int_exp = int(exp)
            if abs(exp - int_exp) < 1e-12:
                operations += [(prime, root, int_exp)]
                found.add(root)
                continue

    operations.sort()  # 'prime' is the first field so it will work properly
    actual_zeros: List[Operation] = []
    for i in operations:
        if not i[0].imag:
            # it's a real zero - we save just the operation;
            # None is a placeholder
            actual_zeros += [(i[2], 0)]
        else:
            # it's a complex 0
            actual_zeros += [(i[1].real, i[2])]
    return actual_zeros

pol_code = get_actual_zeros(roots)
pol_code.append('\0')
print(pol_code)


# localize while loops and if-clauses
def get_blocks(code) -> list:
    code = [i[0] for i in code[:-1]]
    ifs = []
    whiles = []
    for line, cmd in enumerate(code):
        if cmd in {1, 3, 4}:
            # line_start, line_end, if_type
            ifs += [[line, None, cmd]]
        elif cmd == 2:
            # find the last unclosed if-clause
            c = -1
            while ifs[c][1] is not None:
                c -= 1
            ifs[c][1] = line
        elif cmd in {5, 7, 8}:
            # line_start, line_end, while_type
            whiles += [[line, None, cmd]]
        elif cmd == 6:
            # find the last unclosed loop
            c = -1
            while whiles[c][1] is not None:
                c -= 1
            whiles[c][1] = line
    return ifs + whiles

blocks = get_blocks(pol_code)
jumps = {block[0]: block[1] for block in blocks}
jumps.update({block[1]: block[0] for block in blocks})

REG = 0
CMD_PNTR = 0

cmd = pol_code[CMD_PNTR]
while cmd != '\0':
    real, imag = cmd
    if ((real == 1 and not REG > 0)
        or (real == 3 and not REG < 0)
        or (real == 4 and not REG == 0)):
        CMD_PNTR = jumps[CMD_PNTR]
    elif ((real == 5 and not REG > 0)
          or (real == 7 and not REG < 0)
          or (real == 8 and not REG == 0)):
        CMD_PNTR = jumps[CMD_PNTR]
    elif real == 6:
        CMD_PNTR = jumps[CMD_PNTR]
        continue
    else:
        if imag == 1:
            n = int(input())
            REG += n
        elif imag == 2:
            REG -= real
            print(REG)
        elif imag == 3:
            REG *= real
        elif imag == 4:
            REG /= real
        elif imag == 5:
            REG %= real
        elif imag == 6:
            REG **= real
    CMD_PNTR += 1
    cmd = pol_code[CMD_PNTR]
    continue
