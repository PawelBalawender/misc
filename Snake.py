#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
This module implements the snake game
Graphics are made in MatPlotlib
"""
import threading
from typing import Tuple, Callable, Any, Dict, List
import random
import time
import sys

import matplotlib
import matplotlib.pyplot as plt
import matplotlib.patches as patches

# just for type hints
Field = Tuple[int, int]  # [(1, 0), (1, 1), (1, 2)...]
Patch = patches.Rectangle
Figure = matplotlib.figure.Figure
Ax = Any  # idk how to get the type of ax; writes no attribute error


class Board: pass


class GameObject:
    def __init__(self, board: Board, char: str, color: str, fields: list):
        self.board = board
        self.char = char  # unique char that represent the obj on the board
        self.color = color  # color of the patch of this object in GUI
        self.fields = fields  # list of fields on brd occupied by the object
        
        self.board.add_obj(self)


class Board:
    def __init__(self, fig: Figure, ax: Ax, width: int=21, height: int=21):
        self.fig = fig
        self.ax = ax
        self.width = width
        self.height = height
        self.fields = [[0 for _ in range(width)] for _ in range(height)]
        # type: Dict[GameObj: (fields, patches)]
        GameObjDict = Dict[GameObject, Tuple[List[Field], List[Patch]]]
        self.objects: GameObjDict = dict()

        self.xsize = 1  # visual size of one tile
        self.ysize = 1
        self.EMPTY = 0  # it indicates an empty field in the array

    def rm_obj(self, obj: GameObject):
        """
        Find the fields that the given object has occupied on the board,
        clear them and then 'check out' the object from the board
        """
        # transpose to convert [(fld, ptch), ...] to [(fld, fld..), (ptch, ..)]
        for field, patch in zip(*self.objects[obj]):
            x, y = field
            self.fields[y][x] = self.EMPTY
            patch.set_visible(False)
            del patch
        del self.objects[obj]

    def add_obj(self, obj: GameObject):
        """
        'Check in' the given object and place its chars on the board
        """
        _patches = []
        for field in obj.fields:
            x, y = field
            self.fields[y][x] = obj.char
            patch = patches.Rectangle(field, self.xsize, self.ysize, fc=obj.color)
            _patches.append(patch)
            self.ax.add_patch(patch)
        
        self.objects[obj] = [obj.fields, _patches]

    def spawn_food(self):
        """Spawn new piece of food for the snake on the board"""
        range_x, range_y = range(self.width), range(self.height)
        e = self.EMPTY
        free = [(x, y) for x in range_x for y in range_y if self.fields[y][x] == e]
        # If the border was big enough, we could randomize field and then check
        # if it's occupied - would be faster then, but what about a case when
        # the snake is so big that it's on a significant part of the board?
        field = random.choice(free)
        f = Food(self, field)

    def rm_food(self, field: Field):
        """Remove the food that has already been eaten by the snake"""
        # v[0] are the fields. v[1] are the patches
        # actually it usually iterates once, but just to be sure;
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
        if not ((0 <= x < self.width) & (0 <= y < self.height)):
            return False
        if self.fields[y][x] not in [self.EMPTY, 'F']:
            return False
        return True

    def print_asc(self):
        """Nicely print the board in the terminal"""
        border = '+' + ' - '*self.l + '+'
        print(border)
        for row in self.fields:
            print(' '.join(field for field in row))
        print(border)


class Snake(GameObject):
    def __init__(self, board: Board, callback_end: Callable):
        self.fields = [(2, 5),
                (3, 5),
                (4, 5),
                (5, 5),
                (6, 5),
                (7, 5)]
        self.orientation = 0  # 0, 1, 2, 3 == N, E, W, S
        self.speed = 1  # how many fields it moves in 1 turn
        self.callback_end = callback_end

        def foo(*args):
            pass

        self.actions = {
                0: foo,
                'S': foo,
                'F': self.feed,
                }

        super().__init__(board=board, char='S', color='r', fields=self.fields)

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
        return True, otherwise do nothing and return False
        """
        assert 0 <= orient < 4
        
        if {self.orientation, orient} in [{0, 2}, {1, 3}]:  # cant turn 180 deg
            return False

        self.orientation = orient
        return True

    def kill(self):
        """
        Game over
        """
        self.callback_end()

    def manage_events(self):
        """
        Catch up with what's going on on the board. If the snake has hit some food
        or something, do what should have been done
        """
        events = [self.board.fields[y][x] for (x, y) in self.fields]
        for i in events:
            self.actions[i]()

    def feed(self):
        """
        Find in which direction is the tail is going and get the field behind
        Diff betwen pre-tail and tail gotta be same as tail and post-tail
        """
        tail_x, tail_y = self.fields[0]
        pretail_x, pretail_y = self.fields[1]
        dx, dy = pretail_x - tail_x, pretail_y - tail_y
        
        post_tail = self.fields[0][0] + dx, self.fields[0][1] + dy
        self.fields.insert(0, post_tail)
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
    def __init__(self, board: Board, pos: Field):
        super().__init__(board, char='F', color='g', fields=[pos])

if __name__ == '__main__':
    fig, ax = plt.subplots()


    is_alive = threading.Event()
    is_alive.set()

    b = Board(fig, ax, 11, 11)
    s = Snake(board=b, callback_end=is_alive.clear)
    f = Food(b, (10, 10))

    ax.set_xlim([0, b.width])
    ax.set_ylim([0, b.height])
    ax.set_xticks([i for i in range(b.width)])
    ax.set_yticks([i for i in range(b.height)])

    fig.canvas.draw()

    def keyboard_handler(event):
        dirs = {'up': 0, 'right': 1, 'down': 2, 'left': 3}
        try:
            result = s.set_orientation(dirs[event.key])
            if not result:
                print('Wrong direction! Remember that you canot\
rotate 180 degrees')
        except KeyError:
            pass
        except ValueError:
            pass

    def input_handler():
        while is_alive.is_set():
            inp = input()

            try:
                result = s.set_orientation(int(inp))
                if not result:
                    print('Wrong direction! Remember that you cannot\
rotate 180 degrees')
            except ValueError:
                pass

    
    cid = fig.canvas.mpl_connect('key_press_event', keyboard_handler)
    input_thread = threading.Thread(target=input_handler)
    input_thread.start()

    while is_alive.is_set():
        # s.move gotta be in the main thread, cause it deals with mpl methods
        # for instance in patch.set_visible(False) at object deleting
        # it will result in Runtime Error if put in in a thread
        s.move()
        fig.canvas.draw()
        plt.pause(0.4)

    # input is still blocking
    print('Press any button to quit')
    input_thread.join()

