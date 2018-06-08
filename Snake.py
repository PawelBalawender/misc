#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import threading
import time
import sys

import matplotlib.pyplot as plt
import matplotlib.patches as patches
"""
This module implements the snake game
"""


# just a declaration for type annotations
class Board: pass


class GameObject:
    def __init__(self, board: Board, char: str, fields: list):
        self.board = board
        self.char = char
        self.fields = fields        
        self.board.add_obj(self)


class Board:
    def __init__(self, l=11, h=11):
        self.length = l
        self.height = h
        self.fields = [[0 for _ in range(l)] for _ in range(h)]
        self.objects = dict()

        self.EMPTY = 0  # it indicates an empty field in the array

    def rm_obj(self, obj: GameObject):
        """
        Find the fields that the given object has occupied on the board,
        clear them and then 'check out' the object from the board
        """
        for (x, y) in self.objects[obj]:
            self.fields[y][x] = self.EMPTY
        del self.objects[obj]

    def add_obj(self, obj: GameObject):
        """
        'Check in' the given object and place its chars on the board
        """
        self.objects[obj] = obj.fields
        c = obj.char
        for (x, y) in obj.fields:
            self.fields[y][x] = c

    def update(self, obj: GameObject) -> bool:
        """Remove depreciated object and locate the fresh one"""
        self.rm_obj(obj)
        self.add_obj(obj)

    def can_move(self, field: tuple) -> bool:
        """Check if the given point isn't beyond the border and if it's not,
        check if it isn't occupied"""
        x, y = field
        if not ((0 <= x < self.length) & (0 <= y < self.height)):
            return False
        if self.fields[y][x] != self.EMPTY:
            return False
        return True

    def print_asc(self):
        """Nicely print the border in the terminal"""
        border = '+' + ' - '*self.l + '+'
        print(border)
        for i in self.fields:
            for j in i:
                print(j, end=' ')
            print()
        print(border)


class Snake(GameObject):
    def __init__(self, board: Board, callback):
        self.body = [(5, 5), (4, 5)]  # only the head
        self.orientation = 0  # 0, 1, 2, 3 == N, E, W, S
        self.speed = 1  # how many fields it moves in 1 turn
        self.callback = callback
        super().__init__(board, 'S', self.body)

    def new_field(self) -> tuple:
        """
        Calculate the new positon of the snake's head
        if it's gonna make a move
        """
        head_x, head_y = self.fields[-1]
        dx, dy = [(0, 1), (1, 0), (0, -1), (-1, 0)][self.orientation]
        dx *= self.speed
        dy *= self.speed
        return head_x + dx, head_y + dy

    def set_orientation(self, orient: int) -> bool:
        """
        Check if the direction is legal, if it is - turn snake and
        return True, otherwsie do nothing and return False
        """
        if not 0 <= orient < 4:  # no such an orientation
            raise ValueError
        
        if not abs(self.orientation - orient):  # cant turn 180 degrees
            return False

        self.orientation = orient
        return True

    def kill(self):
        """
        Game over
        """
        self.callback(self)

    def move(self) -> bool:
        """
        Calculate the new snake head's position, check
        if the movement is legal and then lose the tail and
        add new head_pos to the body; if isn't legal, game over
        """
        new = self.new_field()
        if not self.board.can_move(new):
            self.kill()
            return False
        self.fields = self.fields[1:] + [new]  # the actual movement
        self.board.update(self)
        return True

if __name__ == '__main__':
    plt.ion()
    fig, ax = plt.subplots()

    is_alive = threading.Event()
    is_alive.set()

    def foo(s: Snake):
        print('Looser!')
        is_alive.clear()

    b = Board()
    s = Snake(b, foo)

    ax.set_xlim([0, b.length])
    ax.set_ylim([0, b.height])
    ax.set_xticks([i for i in range(b.length)])
    ax.set_yticks([i for i in range(b.height)])

    patch = patches.Rectangle(s.fields[-1], 1, 1, fc='g')
    ax.add_patch(patch)
    fig.canvas.draw()

    def uncond():
        while is_alive.is_set():
            inp = input()
            # is_alive could change;
            # if user has just hit the enter, int(inp) would be an error
            if not is_alive.is_set(): return
            s.set_orientation(int(inp))

    t2 = threading.Thread(target=uncond)
    t2.start()

    while is_alive.is_set():
        s.move()
        patch.set_xy(s.fields[-1])
        fig.canvas.draw()
        time.sleep(1)

    print('Press any button to quit')
    t2.join()

