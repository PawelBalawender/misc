#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
from typing import Tuple
import tkinter
import serial
import time

DATA_RES = 8

xlim = ylim = 2 ** DATA_RES - 1

Point = Tuple[int, int]


class Plot:
    def __init__(self, xlim: int=1, ylim: int=1):
        self.root = tkinter.Tk()
        self.root.title('UART data')

        self.xlim = xlim
        self.ylim = ylim

        # relative window size
        size = 50
        self.width = 16 * size
        self.height = 9 * size
        
        self.canvas = tkinter.Canvas(self.root, width=self.width, height=self.height)
        self.background_color = self.canvas['background']

        # relative sizes of axes
        self.ax_xsize = int(self.width * 0.8)
        self.ax_ysize = int(self.height * 0.8)
        # beginning of the plot
        self.ox = int(self.width * 0.1)
        self.oy = int(self.height * 0.9)
        # axes' ends
        self.ox_end = self.ox + self.ax_xsize
        self.oy_end = self.oy - self.ax_ysize
        
        # live plotting flags
        self.stop = -1
        self.restart = -2

        self.center_window()
        self.draw_axes()

        self.canvas.pack(expand=tkinter.YES, fill=tkinter.BOTH)
        #self.root.mainloop()

    def center_window(self) -> None:
        win_w = self.root.winfo_screenwidth()
        win_h = self.root.winfo_screenheight()
        cx = win_w//2 - self.width//2
        cy = win_h//2 - self.height//2
        self.root.geometry('{}x{}+{}+{}'.format(self.width, self.height, cx, cy))

    def draw_axes(self) -> None:
        # OX axis
        self.canvas.create_line(self.ox, self.oy, self.ox_end, self.oy)
        # OY axis
        self.canvas.create_line(self.ox, self.oy, self.ox, self.oy_end)

    def scale_x(self, x: int) -> None:
        # position some arbitrary plot's x value to a real x coord in the window
        return int(self.ox+1 + (x / self.xlim) * self.ax_xsize)

    def scale_y(self, y: int) -> None:
        return int(self.oy-1 - (y / self.ylim) * self.ax_ysize)

    def plot(self, a: Point, b: Point) -> None:
        a = self.scale_x(a[0]), self.scale_y(a[1])
        b = self.scale_x(b[0]), self.scale_y(b[1])
        self.canvas.create_line(a[0], a[1], b[0], b[1], fill='red')

    def clear_plot(self) -> None:
        coords = self.ox+1, self.oy-1, self.ox_end+1, self.oy_end-1
        kwargs = dict(fill=self.background_color, outline=self.background_color)
        self.canvas.create_rectangle(*coords, **kwargs)

    def plot_live(self, auto_flush: bool=False):
        auto_flush = False
        data = None
        while data != self.stop:
            # get init x, y
            x, y = yield
            last_x, last_y = x,y 

            # will restart if there was a request, or x has exceeded the plot
            while (x, y) not in {self.restart, self.stop}:
                x, y = yield
                
                self.plot((last_x, last_y), (x, y))
                last_x, last_y = x, y
                self.canvas.update()


class Port:
    def __init__(self, device: str, baudrate: int):
        self.port = serial.Serial('/dev/ttyACM0', baudrate=9600)
        if not self.port.is_open:
            self.port.open()

    def take_data(self, fresh: bool=False) -> int:
        if fresh:
            self.port.reset_input_buffer()
            x = self.port.readline()
            print('cleanin:', x)

        values = None
        while values is None:
            values = self.port.readline()
            print(values)
            values = values[:-1]  # pop \n
            values = values[:4], values[4:]
            try:
                x = int.from_bytes(values[0], byteorder='big', signed=False)
                y = int.from_bytes(values[1], byteorder='big', signed=False)
                values = x, y
            except ValueError:
                print('Data invalid')
            except IndexError:
                print('Data invalid')
        return values


import math
v = 16
x_min, x_max = v+1, xlim-v
y_min, y_max = v+1, ylim-v
x, y = x_min, y_min
car_dir = math.pi/4-0.3
x_ok=y_ok=0

if __name__ == '__main__':
    plot = Plot(xlim=xlim, ylim=ylim)
    stm = Port(device='/dev/ttyACM0', baudrate=9600)

    # start plotting
    control = plot.plot_live(auto_flush=True)
    next(control)
    control.send(stm.take_data(fresh=True))
    while True:
        x, y = stm.take_data(fresh=True)
        print(x, y)
        control.send((x, y))
        time.sleep(0.1)

    """
    while True:
        control.send((x, y))
        time.sleep(0.05)
        
        next_x = x + math.cos(car_dir) * v
        next_y = y + math.sin(car_dir) * v

        x_ok = x_min < next_x < x_max
        y_ok = y_min < next_y < y_max
        if not x_ok:
            car_dir *= -1
            car_dir += math.pi
            continue
        if not y_ok:
            car_dir *= -1
            continue

        x, y = next_x, next_y
        continue

        if not x_ok and not y_ok:
            print(1)
            car_dir *= -1
            if not x_ok:
                car_dir += math.pi
            continue
        
        x, y = next_x, next_y



        #control.send((x, y))
    """




    plot.root.mainloop()


