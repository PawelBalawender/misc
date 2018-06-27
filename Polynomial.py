#!/usr/bin/env python
# -*- coding: UTF-8 -*-
"""
This module implements an interpreter for the Polynomial esoteric language
"""
from collections import namedtuple
from typing import List, Tuple, Set

import numpy as np

Operation = namedtuple('Operation', ['operation', 'operand'])


def clear_roots(_roots: np.ndarray) -> np.ndarray:
    """Eliminate almost-zeros and complex conjugates"""
    _roots.real[abs(_roots.real) < 1e-11] = 0
    _roots.imag[abs(_roots.imag) < 1e-11] = 0
    _roots = _roots[roots.imag >= 0]  # if b in (a+bi) < 0, it's a NOP for us
    return _roots


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


def is_almost_integer(x: float, tol=1e-11) -> bool:
    """
    Check if a float is almost an integer
    """
    return abs(x - int(round(x, 8))) < tol


def get_actual_zeros(_roots: np.ndarray) -> List[complex]:
    """
    Essential thing about parsing Polynomial code - take original polynomial's
    zeros and find the actual operations that they correspond to
    """
    # todo: the complexity can be decreased from primes*_roots to some log
    # todo: by checking the roots that have been already calculated
    prime_gen = generate_primes()
    primes = [next(prime_gen) for _ in _roots]

    commands = [0 for _ in _roots]
    for prime in primes:
        for root in _roots:
            if root.imag:  # a+bi -> a + (p^b)i
                exp = np.log(root.imag) / np.log(prime)
            else:  # a -> p^a
                if root.real < 0:
                    # -root.real to get abs and -np.log to save the orig. sign
                    exp = -np.log(-root.real) / np.log(prime)
                else:
                    exp = np.log(root.real) / np.log(prime)

            # if exp is int or almost-int - we found it!
            # int_exp is our actual zero - so the operation
            if is_almost_integer(exp):
                int_exp = int(round(exp, 8))
                if root.imag:
                    operand, operator = root.real, int_exp * 1j
                    # eliminate operand==0.99999987 etc.
                    if is_almost_integer(operand):
                        operand = int(round(operand, 8))
                else:
                    operand, operator = 0, int_exp
                # sorting by making this lookup in the primes' order table
                commands[primes.index(prime)] = complex(operand + operator)
    return commands


def translate_to_python(polynomial_code: List[complex]) -> str:
    translation = {
        -1j: 'print(chr(ACC), end=\'\')',
        -2j: 'ACC = ord(input())',
        1j: 'ACC += {}',
        2j: 'ACC -= {}',
        3j: 'ACC *= {}',
        4j: 'ACC /= {}',
        5j: 'ACC %= {}',
        6j: 'ACC **= {}',
        1: 'if ACC > 0:',
        2: '',
        3: 'if ACC < 0:',
        4: 'if not ACC:',
        5: 'while ACC > 0:',
        6: '',
        7: 'while ACC < 0:',
        8: 'while not ACC:'
    }

    python_code = 'ACC = 0\n'

    indent = 0
    for cmd in pol_code:
        if cmd == (0 + 1j):
            cmd = -1j
        elif cmd == (0 + 2j):
            cmd = -2j

        python_code += '\t' * indent
        if cmd.imag:
            python_code += translation[int(cmd.imag) * 1j].format(int(cmd.real))
        else:
            python_code += translation[int(cmd.real)]
        python_code += '\n'

        if cmd in {1, 3, 4, 5, 7, 8}:
            indent += 1
        elif cmd in {2, 6}:
            indent -= 1

    return python_code


def translate_to_pol(zeros: List[complex]) -> str:
    """
    Translate the zeros that we need for the program to a Polynomial code
    """
    # fixme: what's happening when the pol has a coefficient 0 somewhere?
    # fixme: what's happening when the pol has a complex/imaginary coefficient?
    prime_gen = generate_primes()
    prime = 2  # just a declaration in case of complex conj. is the first stmt
    result = 1
    for zero in zeros:
        #  zeros: 1, i, -i, 2 -> zeros with primes: 2^1, 0+3^1j + 0-3^1j + 5^2
        if zero.imag > 0:
            # (a+bi) -> a+(p^b)i
            prime = next(prime_gen)
            zero = zero.real + (prime ** zero.imag) * 1j
        elif zero.imag < 0:
            # it's a complex conjugate so we do not get a next prime
            zero = -(zero.real + (prime ** -zero.imag) * 1j)
        elif not zero.imag:
            # x -> p^x
            prime = next(prime_gen)
            zero = prime ** zero

        # zeros: 2, 3i, -3i, 25 -> (x - 2)(x - 3i)(x + 3i)(x -25)
        zero = (1, -zero)
        result = np.polymul(result, zero)

    print(result)

    # now from (x - 2)(x - 3i)(x + 3i)(x - 25) we have a polynomial's coeffs:
    # result = [1, -27, 59, -243, 450], so we just make a string from it
    final = 'f(x) ='
    for i, coefficient in enumerate(result):
        if not coefficient:
            continue

        x_power = len(result) - 1 - i
        if x_power > 1:
            power_string = f'x^{x_power}'
        elif x_power == 1:
            power_string = 'x'
        elif not x_power:
            power_string = ''

        num = int(coefficient.real)
        sgn = ['- ', '+ '][num > 0]
        # if it's the first element, omit the +
        if sgn == '+ ' and not i:
            sgn = ' '

        if num != 1:
            coeff_string = ' {}{}'.format(sgn, abs(num))
        else:
            coeff_string = str(sgn)

        string = coeff_string + power_string

        final += string
    return final

# quick testing
assert is_almost_integer(0.99999999999997102)
assert not is_almost_integer(0.99)

prog = 'f(x) = x^10 - 4827056x^9 + 1192223600x^8 - 8577438158x^7 + 958436165464x^6 - 4037071023854x^5 + 141614997956730x^4 - 365830453724082x^3 + 5225367261446055x^2 - 9213984708801250x + 21911510628393750'

# todo: parser could use yielding and save some memory
pairs = parse_polynomial(prog)
coefficients = get_coefficients(pairs)
roots = np.roots(coefficients)
roots = clear_roots(roots)
pol_code = get_actual_zeros(roots)
python_code = translate_to_python(pol_code)

# pol = '''
# 1, 1
# 1, -1
# 5, 0
# 0, 2
# 0, -2
# 0, 1
# 0, -1
# 1, 1
# 1, -1
# 6, 0
# '''
#
# pol = pol.split('\n')
# pol = pol[1:-1]  # cut leading and trailing ''s
# pol = [i.split(', ') for i in pol]
# pol = [complex(int(i[0]), int(i[1])) for i in pol]
# pol = translate_to_pol(pol)
# prog = pol
print(python_code)