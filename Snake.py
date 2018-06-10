#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import threading
import random
import time
import sys

import matplotlib.pyplot as plt
import matplotlib.patches as patches
"""
This module implements the snake game

under construction!
todo:
    making patches in Board
"""


# just a declaration for type annotations
class Board: pass


class GameObject:
    def __init__(
            self,
            board: Board,
            char: str,
            color: str,
            fields: list):
        self.board = board
        self.char = char
        self.color = color
        self.fields = fields        
        self.board.add_obj(self)


class Board:
    def __init__(self, fig, ax, l=11, h=11):
        self.fig = fig
        self.ax = ax
        self.length = l
        self.height = h
        self.fields = [[0 for _ in range(l)] for _ in range(h)]
        # BoardObject: Tuple[List[tuple[int]], List[patches.Rectangle]]
        self.objects = dict()

        self.EMPTY = 0  # it indicates an empty field in the array

    def rm_obj(self, obj: GameObject):
        """
        Find the fields that the given object has occupied on the board,
        clear them and then 'check out' the object from the board
        """
        # iterate over transposed version ([flds], [ptchs] -> [f0, p0...])
        for field, patch in zip(*self.objects[obj]):
            x, y = field
            self.fields[y][x] = self.EMPTY
            patch.set_visible(False)
        del self.objects[obj]

    def add_obj(self, obj: GameObject):
        """
        'Check in' the given object and place its chars on the board
        """
        c = obj.char
        col = obj.color

        _patches = []
        for i in obj.fields:
            self.fields[i[1]][i[0]] = c
            patch = patches.Rectangle(i, 1, 1, fc=col)
            _patches.append(patch)
            self.ax.add_patch(patch)
        self.objects[obj] = [obj.fields, _patches]

    def spawn_food(self):
        free = []
        for y, row in enumerate(self.fields):
            for x, f in enumerate(row):
                if f == self.EMPTY:
                    free.append((x, y))

        field = random.choice(free)
        f = Food(self, field)

    def rm_food(self, field: tuple):
        objs = [k for (k, v) in self.objects.items() if field in v[0]]
        for obj in objs:
            self.rm_obj(obj)
            self.spawn_food()
        
    def update(self, obj: GameObject):
        """Remove depreciated object and locate the fresh one"""
        self.rm_obj(obj)
        self.add_obj(obj)

    def can_move(self, field: tuple) -> bool:
        """Check if the given point isn't beyond the border and if it's not,
        check if it isn't occupied"""
        x, y = field
        if not ((0 <= x < self.length) & (0 <= y < self.height)):
            return False
        if self.fields[y][x] not in [0, 'F']:
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
        self.fields = [(2, 5),
                (3, 5),
                (4, 5),
                (5, 5),
                (6, 5),
                (7, 5)]
        self.orientation = 0  # 0, 1, 2, 3 == N, E, W, S
        self.speed = 1  # how many fields it moves in 1 turn
        self.callback = callback

        self.actions = {
                0: lambda *x: None,
                'S': lambda *x: None,
                'F': self.feed,
                }

        # self.patches = []
        # for field in self.fields:
            # patch = patches.Rectangle(field, 1, 1, fc='g')
            # self.patches.append(patch)
        super().__init__(board, 'S', 'r', self.fields)

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
        
        if {self.orientation, orient} in [{0, 2},{1, 3}]:  # cant turn 180 dg
            return False

        self.orientation = orient
        return True

    def kill(self):
        """
        Game over
        """
        self.callback(self)

    def manage_events(self):
        events = [self.board.fields[y][x] for (x, y) in self.fields]
        for i in events:
            self.actions[i]()

    def feed(self):
        # inverted vector from new_field(); we add new block to the tail
        dx, dy = [(0, -1), (-1, 0), (0, 1), (1, 0)][self.orientation]
        tail = self.fields[0][0] + dx, self.fields[0][1] + dy
        self.fields = [tail] + self.fields
        self.board.rm_food(self.fields[-1])
        self.board.update(self)

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

        # the tail becomes the head - no need to refresh every patch
        # self.patches[0].set_xy(new)
        # self.patches = self.patches[1:] + [self.patches[0]]

        self.manage_events()
        self.board.update(self)
        return True


class Food(GameObject):
    def __init__(self, board: Board, pos: tuple):
        super().__init__(board, 'F', 'g', [pos])

if __name__ == '__main__':
    plt.ion()
    fig, ax = plt.subplots()

    is_alive = threading.Event()
    is_alive.set()

    def foo(s: Snake):
        print('Looser!')
        is_alive.clear()

    b = Board(fig, ax)
    s = Snake(b, foo)
    f = Food(b, (3, 3))

    ax.set_xlim([0, b.length])
    ax.set_ylim([0, b.height])
    ax.set_xticks([i for i in range(b.length)])
    ax.set_yticks([i for i in range(b.height)])

    # patch = patches.Rectangle(s.fields[-1], 1, 1, fc='g')
    # for patch in s.patches:
        # ax.add_patch(patch)
    # ax.add_patch(patches.Rectangle(f.fields[0], 1, 1, fc='r'))
    fig.canvas.draw()

    def uncond():
        while is_alive.is_set():
            inp = input()
            # is_alive could change during the input
            if not is_alive.is_set(): return
            try:
                s.set_orientation(int(inp))
            except ValueError:
                continue


    t2 = threading.Thread(target=uncond)
    t2.start()

    while is_alive.is_set():
        s.move()
        # patch.set_xy(s.fields[-1])
        fig.canvas.draw()
        time.sleep(0.1)

    print('Press any button to quit')
    t2.join()

