#!/usr/bin/env python
# -*- coding: UTF-8 -*-
"""
under construction!

This module implements the snake game
"""

class Board(object):
    def __init__(self, l=10, h=10):
        self.length = l
        self.height = h
        self.fields = [[0 for _ in range(l)] for _ in range(h)]

    def __getitem__(self, pos):
        return self.fields[pos[1]][pos[0]]


class Snake():
    def __init__(self, board):
        self.head = 0, 0
        self.body = [self.head]
        self.orientation = 0  # 0, 1, 2, 3 == N, E, W, S
        self.speed = 1

        self.board = board  # the board that the snake "lives" on

    def collision(self):
        d, x, y = self.orientation, self.head[0], self.head[1]
        l, h = self.board.length, self.board.height

        # check collision with board's boundaries
        if d == 0:
            if y == h - 1:
                return True
            next_field = (x, y + 1)
        if d == 1:
            if x == l - 1:
                return True
            next_field = (x + 1, y)
        if d == 2:
            if y == 0:
                return True
            next_field = (x, y - 1)
        if d == 3:
            if x == 0:
                return True
            next_field = (x - 1, y)

        if self.board[next_field] != 0:
            return True
        return False

    def move(self, leap=1, direction=0):
        if not self.orientation:
            self.body = [(i[0], i[1] + 1) for i in self.body]
        elif self.orientation == 1:
            self.body = [(i[0] + 1, i[1]) for i in self.body]
        elif self.orientation == 2:
            self.body = [(i[0], i[1] - 1) for i in self.body]
        elif self.orientation == 3:
            self.body = [(i[0] - 1, i[1]) for i in self.body]

        self.board.update(self.body, 0)


class TestBoard:
    def __init__(self):
        pass


class TestSnake:
    def __init__(self):
        pass

    def run(self):
        self.test_init()
        self.test_check_collision()
        self.test_move()

    def test_init(self):
        snake = Snake(Board())
        assert snake.head == (0, 0)
        assert snake.body == [snake.head]
        assert snake.speed == 1
        assert snake.orientation == 0

    def test_check_collision(self):
        snake = Snake(Board())

        snake.head = (0, 0)
        snake.body = [snake.head]
        snake.orientation = 2
        assert snake.collision()

        snake.orientation = 3
        assert snake.collision()

        snake.head = (0, 1)
        snake.body = [(0, 0), (1, 0), (2, 0),
                      (2, 1), (2, 2),
                      (1, 2), (0, 2),
                      snake.head]
        snake.orientation = 2
        assert snake.collision()

        snake.head = (0, 1)
        snake.body = [(0, 0), (1, 0),
                      (1, 1), snake.head]
        snake.orientation = 3
        assert snake.collision()

        snake.head = (4, 3)
        snake.body = [(3, 3), (3, 4), (3, 5), (3, 6),
                      (4, 6), (4, 5), (4, 4), snake.head]
        snake.orientation = 2
        assert not snake.collision()

    def test_move(self):
        snake = Snake(Board())

        snake.head = (0, 0)
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
        assert snake.body == body[1:] + [(6, 4)]


def main():
    pass


if __name__ == '__main__':
    TestSnake().run()
    main()
=======
	def __init__(self, l=10, h=10):
		self.length = l
		self.height = h
		self.fields = [[0 for _ in range(l)] for _ in range(h)]
	
	def __getitem__(self, pos):
		return self.fields[pos[1]][pos[0]]


class Snake():
	def __init__(self, board):
		self.head = 0, 0
		self.body = [self.head]
		self.orientation = 0  # 0, 1, 2, 3 == N, E, W, S
		self.speed = 1
		
		self.board = board  # the board that the snake "lives" on
	
	def collision(self):
		d, x, y = self.orientation, self.head[0], self.head[1]
		l, h = self.board.length, self.board.height
		
		# check collision with board's boundaries
		if d == 0:
			if y == h-1:
				return True
			next_field = (x, y + 1)
		if d == 1:
			if x == l-1:
				return True
			next_field = (x + 1, y)
		if d == 2:
			if y == 0:
				return True
			next_field = (x, y - 1)
		if d == 3:
			if x == 0:
				return True
			next_field = (x - 1, y)
		
		if self.board[next_field] != 0:
			return True
		return False
		

	def move(self, leap=1, direction=0):
		if not self.orientation:
			self.body = [(i[0], i[1]+1) for i in self.body]
		elif self.orientation == 1:
			self.body = [(i[0]+1, i[1]) for i in self.body]
		elif self.orientation == 2:
			self.body = [(i[0], i[1]-1) for i in self.body]
		elif self.orientation == 3:
			self.body = [(i[0]-1, i[1]) for i in self.body]
		
		self.board.update(self.body, 0)


class TestBoard:
	def __init__(self):
		pass


class TestSnake:
	def __init__(self):
		pass
	
	def run(self):
		self.test_init()
		self.test_check_collision()
		self.test_move()

	def test_init(self):
		snake = Snake(Board())
		assert snake.head == (0, 0)
		assert snake.body == [snake.head]
		assert snake.speed == 1
		assert snake.orientation == 0
	
	def test_check_collision(self):
		snake = Snake(Board())

		snake.head = (0, 0)
		snake.body = [snake.head]
		snake.orientation = 2
		assert snake.collision()
		
		snake.orientation = 3
		assert snake.collision()
		
		snake.head = (0, 1)
		snake.body = [(0, 0), (1, 0), (2, 0),
				(2, 1), (2, 2),
				(1, 2), (0, 2),
				snake.head]
		snake.orientation = 2
		assert snake.collision()
		
		snake.head = (0, 1)
		snake.body = [(0, 0), (1, 0),
				(1, 1), snake.head]
		snake.orientation = 3
		assert snake.collision()
		
		snake.head = (4, 3)
		snake.body = [(3, 3), (3, 4), (3, 5), (3, 6),
				(4, 6), (4, 5), (4, 4), snake.head]
		snake.orientation = 2
		assert not snake.collision()
	
	def test_move(self):
		snake = Snake(Board())
		
		snake.head = (0, 0)
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
		assert snake.body == body[1:] + [(6, 4)]

def main():
	pass

if __name__ == '__main__':
	TestSnake().run()
	main()
