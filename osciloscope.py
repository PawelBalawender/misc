#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
from tkinter import *
import serial
import time

xlim = 100
ylim = 4095

stm = serial.Serial('/dev/ttyACM0', baudrate=9600)
if not stm.is_open:
    stm.open()

master = Tk()
master.title('Data')

WINSIZE = 50
width = 16 * WINSIZE
height = 9 * WINSIZE
w = Canvas(master, width=width, height=height)
bg = w['background']

# relative sizes of axes
x_size = int(width * 0.8)
y_size = int(height * 0.8)
# beginning of the plot
Ox = int(width * 0.1)
Oy = int(height * 0.9)


def center_window() -> None:
    win_width = master.winfo_screenwidth()
    win_height = master.winfo_screenheight()
    cx = win_width//2 - width//2
    cy = win_height//2 - height//2
    master.geometry('{}x{}+{}+{}'.format(width, height, cx, cy))


def draw_axes() -> None:
    # OX axis
    w.create_line(Ox, Oy, Ox+x_size, Oy)
    # OY axis
    w.create_line(Ox, Oy, Ox, Oy-y_size)
    w.pack(expand=YES, fill=BOTH)


def scale_x(x):
    return Ox + (x / xlim) * x_size


def scale_y(y):
    return Oy - (y / ylim) * y_size


def plot(a, b) -> None:
    x1, y1 = a
    x2, y2 = b
    x1, x2 = scale_x(x1), scale_x(x2)
    y1, y2 = scale_y(y1), scale_y(y2)
    w.create_line(x1, y1, x2, y2, fill='red')


def clear_plot() -> None:
    coords = Ox+1, Oy-1, (Ox+x_size)-1, (Oy-y_size)+1
    kwargs = dict(fill=bg, outline=bg)
    w.create_rectangle(*coords, **kwargs)

def gen():
    last = yield
    x = 0
    while True:
        new = yield
        plot((x, last), (x+1, new))
        last = new
        x += 1
        w.update()


def take_data(port) -> int:
    y = None
    while y is None:
        x = port.readline()
        try:
            y = int(x)
        except ValueError:
            pass
    return y


if __name__ == '__main__':
    center_window()
    draw_axes()

    # start plotting
    while True:
        g = gen()
        next(g)
        
        # send the init y
        y = take_data(stm)
        g.send(y)

        for _ in range(xlim):
            g.send(take_data(stm))
            stm.reset_input_buffer()
        
        clear_plot()
    master.mainloop()

