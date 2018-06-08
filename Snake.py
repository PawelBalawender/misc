#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import threading
import time
import sys

import matplotlib.pyplot as plt
import matplotlib.patches as patches
"""
under construction!
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
    def __init__(self, l=10, h=10):
        self.length = l
        self.height = h
        self.fields = [[0 for _ in range(l)] for _ in range(h)]
        self.objects = dict()

    def __getitem__(self, pos):
        return self.fields[pos[1]][pos[0]]

    def rm_obj(self, obj: GameObject):
        for field in self.objects[obj]:
            self.fields[field[1]][field[0]] = 0
        del self.objects[obj]

    def add_obj(self, obj: GameObject):
        self.objects[obj] = obj.fields
        c = obj.char
        for field in obj.fields:
            self.fields[field[1]][field[0]] = c

    def update(self, obj: GameObject) -> bool:
        self.rm_obj(obj)
        self.add_obj(obj)

    def can_move(self, field: tuple) -> bool:
        x, y = field
        res = (0 <= x < 10) & (0 <= y < 10)  # isn't it beyond the board?
        res &= self.fields[y][x] == 0  # is it empty?
        return res

    def print_asc(self):
        border = '+' + ' - '*self.l + '+'
        print(border)
        for i in self.fields:
            for j in i:
                print(j, end=' ')
            print()
        print(border)


class Snake(GameObject):
    def __init__(self, board):
        self.body = [(0, 0)]  # only the head
        self.orientation = 0  # 0, 1, 2, 3 == N, E, W, S
        self.speed = 1  # how many fields it moves in 1 turn
        self.is_alive = True
        super().__init__(board, 'S', self.body)

    def new_field(self):
        head_x, head_y = self.fields[-1]
        dx, dy = [(0, 1), (1, 0), (0, -1), (-1, 0)][self.orientation]
        dx *= self.speed
        dy *= self.speed
        return head_x + dx, head_y + dy

    def set_orientation(self, orient):
        if not 0 <= orient < 4:  # no such an orientation
            raise ValueError
        
        if not abs(self.orientation - orient):  # cant turn 180 degrees
            return False

        self.orientation = orient
        return True

"""
    # todo: move it to the Border cls
    def collision(self) -> bool:
        head_x, head_y = self.fields[-1]
        dx, dy = [(0, 1), (1, 0), (0, -1), (-1, 0)][self.orientation]
        new_x, new_y = head_x + dx, head_y + dy

        res = 0 <= new_x < 10 & 0 <= new_y < 10  # isn't it beyond the board?
        res &= self.board.fields[new_y][new_x] == 0  # is it empty?
        return res
"""
    def kill(self):
        print('Looser!')
        self.is_alive = False

    def move(self) -> bool:
        # calculate the new pos of the head; the tail will be lost
        # head_x, head_y = self.fields[-1]
        # dx, dy = [(0, 1), (1, 0), (0, -1), (-1, 0)][self.orientation]
        # dx *= self.speed
        # dy *= self.speed
        # new_x, new_y = head_x + dx, head_y + dy
        new = self.new_field()
        if not self.board.can_move(new):# new_x, new_y):
            self.kill()
            return False

        self.fields = self.fields[1:] + [new]#(new_x, new_y)]
        self.board.update(self)
        return True

class TestBoard:
    def run():
        TestBoard.test_add_rm_obj()

    def test_add_rm_obj():
        b = Board()
        o = GameObject(b, 'o', [(0, 0), (1, 0), (1, 1), (2, 1)])
        b.add_obj(o)

        # test: add_obj
        assert b.objects == {o: o.fields}
        x = b.fields
        f = x[0][0] + x[0][1] + x[1][1] + x[1][2]
        assert f == 'oooo'
        assert sum(i.count('o') for i in b.fields) == 4

        # test: rm_obj
        b.rm_obj(o)
        assert b.objects == dict()
        assert b.fields == [[0 for _ in range(10)] for i in range(10)]

class TestSnake:
    def run():
        TestSnake.test_init()
        TestSnake.test_check_collision()
        TestSnake.test_move()

    def test_init():
        snake = Snake(Board())
        assert snake.head == (0, 0)
        assert snake.body == [snake.head]
        assert snake.speed == 1
        assert snake.orientation == 0

    def test_check_collision():
        snake = Snake(Board())

        snake.head = (0, 0)
        snake.body = [snake.head]
        snake.orientation = 2
        snake.board.update(snake)
        assert snake.collision()

        snake.orientation = 3
        assert snake.collision()

        snake.head = (0, 1)
        snake.body = [(0, 0), (1, 0), (2, 0),
                      (2, 1), (2, 2),
                      (1, 2), (0, 2),
                      snake.head]
        snake.orientation = 2
        snake.board.update(snake)
        assert snake.collision()

        snake.head = (0, 1)
        snake.body = [(0, 0), (1, 0),
                      (1, 1), snake.head]
        snake.orientation = 3
        snake.board.update(snake)
        assert snake.collision()

        snake.head = (4, 3)
        snake.body = [(3, 3), (3, 4), (3, 5), (3, 6),
                      (4, 6), (4, 5), (4, 4), snake.head]
        snake.orientation = 2
        snake.board.update(snake)
        assert not snake.collision()

    def test_move():
        snake = Snake(Board())
        # snake body: [head] = [(0, 0)]
        # snake orient: 0

        snake.move(0)
        assert snake.head == (0, 1)

        snake.move(1)
        assert snake.head == (1, 1)

        snake.move(2)
        assert snake.head == (1, 0)
        assert not snake.move(2)
        assert snake.head == (1, 0)

        assert snake.body == [snake.head]

        snake.head = (5, 4)
        body = [(0, 0), (1, 0), (2, 0),
                (2, 1), (3, 1), (4, 1),
                (4, 2), (4, 3), (4, 4), snake.head]
        snake.body = body
        snake.move(1)
        assert snake.head == (6, 4)
        assert snake.body == [(i[0]+1, i[1]) for i in body]

if __name__ == '__main__':
    # TestSnake.run()
    # TestBoard.run()
    
    plt.ion()
    fig, ax = plt.subplots()
    ax.set_xlim([0, 10])
    ax.set_ylim([0, 10])
    ax.set_xticks([i for i in range(10)])
    ax.set_yticks([i for i in range(10)])

    b = Board()
    s = Snake(b)

    patch = patches.Rectangle(s.fields[-1], 1, 1, fc='g')
    ax.add_patch(patch)
    fig.canvas.draw()

    def uncond():
        while s.is_alive:
            s.set_orientation(int(input()))

    t2 = threading.Thread(target=uncond)
    t2.start()

    while s.is_alive:
        print(s.fields[-1])
        s.move()
        patch.set_xy(s.fields[-1])
        fig.canvas.draw()
        time.sleep(1)
    
    plt.show()
    t2.join()
