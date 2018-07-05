#!/usr/bin/env python3
#-*- coding: UTF-8 -*-
"""
This module implements a function that obfuscates obtaining the given string
"""
from typing import List, Tuple
import sys


def return_same_string(string: str) -> str:
    """It returns the same string as input, but led through an obfuscated way"""
    asciis = [(0, i) for i in map(ord, string)]

    lengths = [(name, len(val.__doc__)) for (name, val) in sys.__dict__.items() if val.__doc__]
    
    lengths += asciis
    lengths.sort(key=lambda x: x[1])

    close = []
    for i in asciis:
        index = lengths.index(i)
        if not index:
            close += [(lengths[1][0], lengths[1][1] - i[1])]
        else:
            close += [(lengths[index - 1][0], lengths[index - 1][1] - i[1])]
    
    chars = [chr(len(sys.__dict__[name].__doc__) - diff) for (name, diff) in close]
    string = ''.join(chars)
    return string


def return_same_string(s):
    a = [(0,i)for i in map(ord, s)]
    l = [(n, len(v.__doc__)) for(n, v) in sys.__dict__.items() if v.__doc__]    
    l += a
    l.sort(key=lambda x: x[1])

    close = []
    for i in a:
        index = l.index(i)
        if not index:
            close += [(l[1][0], l[1][1] - i[1])]
        else:
            close += [(l[index - 1][0], l[index - 1][1] - i[1])]
    
    c = [chr(len(sys.__dict__[name].__doc__) - diff) for (name, diff) in close]
    s = ''.join(c)
    return s


string = 'Hello!'
string = return_same_string(string)
print(string)

