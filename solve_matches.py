#!/usr/bin/env python3
# -*- coding: UTF-8 -*-

digits = [
        0b1111111 & 0b1110111,  # 0; every but 3
        1<<2 | 1<<5,  # 1; only 2 and 5
        0b1111111 & 0b1011101,  # 2; every but 1 and 5
        0b1111111 & 0b1101101,  # 3; every but 1 and 4
        0b1111111 & 0b0101110,  # 4; every but 0, 4, 5
        0b1111111 & 0b1101011,  # 5; every but 2, 4
        0b1111111 & 0b1111011,  # 6; every but 2
        1<<0 | 1<<2 | 1<<5,  # 7; only 0, 2, 5
        0b1111111,  # 8; every
        0b1111111 & 0b1101111  # 9; every but 4
        ]

def get_close_digits():
    close = []
    for i in range(10):  # there are 10 digits
        digit = digits[i]
        close += [[i]]
        for j in range(-1, 7):  # size of each set is 7 bits
            # for j==-1 it's unchanged, for higher j's: j-th bit is ON
            d = digit | (1 << j) if j >= 0 else digit
            if d == digit:
                continue

            for k in range(-1, 7):
                # for k==-1 it;s unchanged, else: k-th bit is OFF
                mask = 0b1111111 ^ (1 << k ) if k >= 0 else 0b1111111
                new = d & mask
                if new == digit:
                    continue

                if new in digits:
                    close[i] += [digits.index(new)]
    return {i: set(j) for (i,j) in enumerate(close)}


def get_matches_count(char: str) -> int:
    sched = str(bin(char))
    return sum(1 for i in sched[2:] if i == '1')
g = get_matches_count
assert g(0b0101) == 2
assert g(0b0) == 0
assert g(0b111) == 3

prices = {i: get_matches_count(digits[i]) for i in range(10)}
prices.update({'+': 2, '-': 1, '=': 2})
close_digits = get_close_digits()

raw_eq = input()
assert len(raw_eq) == 5
eq_numbs = list(map(int, raw_eq[::2]))  # cannot be generator, it's used a few times
eq_signs = raw_eq[1::2]
assert len(eq_signs) == 2

orig_num_price = sum(prices[i] for i in eq_numbs)
orig_sgn_price = sum(prices[i] for i in eq_signs)
assert orig_sgn_price in {3, 4}

eq_numbs = list(eq_numbs)
possibilities = [close_digits[i] for i in eq_numbs]
assert len(possibilities) == 3

ok = []
for i in possibilities[0]:
    for j in possibilities[1]:
        for k in possibilities[2]:
            new_num_price = sum([prices[dgt] for dgt in [i, j, k]])
            bil = orig_num_price - new_num_price  # |bil| <= 3
            # we cannot organise more than 1 match
            if bil in {-3, -2, 2, 3}: continue
            # if our new digits need a match, but we don't have one
            elif bil == -1 and orig_sgn_price == 3: continue
            # if our digits gotta give us a match, but we don't have a place
            elif bil == 1 and orig_sgn_price == 4: continue

            # if we can make it ok with matches count:
            elif bil == -1 and orig_sgn_price == 4:
                # it inducts that the signs are +=; change + to - and try
                if i - j == k: ok += [f'{i}-{j}={k}']
                continue
            elif bil == 1 and orig_sgn_price == 3:
                # signs are -=; change - to +
                if i + j == k: ok += [f'{i}+{j}=k']
                continue
            elif not bil:
                # can do nothing with signs
                if eq_signs[0] == '+' and i + j == k: ok += [f'{i}+{j}={k}']
                elif eq_signs[0] == '-' and i - j == k: ok += [f'{i}i{j}={k}']
print(ok)

