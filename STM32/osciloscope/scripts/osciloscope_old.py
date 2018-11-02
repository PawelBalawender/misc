#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
from typing import Tuple
import tkinter
import serial
import time

# ADC resolution in bits
DATA_RES = 8

xlim = 2500
ylim = 2 ** DATA_RES - 1

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
        data = None
        while data != self.stop:
            data = yield
            last = data

            x = 0
            # will restart if there was a request, or x has exceeded the plot
            while data not in {self.restart, self.stop}:
                if auto_flush and x >= self.xlim:
                    break
                data = yield
                
                self.plot((x, last), (x+1, data))
                last = data
                x += 1
                self.canvas.update()

            if auto_flush:
                self.clear_plot()

class Port:
    def __init__(self, device: str, baudrate: int):
        self.port = serial.Serial('/dev/ttyACM0', baudrate=9600)
        if not self.port.is_open:
            self.port.open()

    def take_data(self, fresh: bool=False) -> int:
        if fresh:
            self.port.reset_input_buffer()

        value = None
        while value is None:
            temp = self.port.read()
            try:
                value = int.from_bytes(temp, byteorder='little', signed=False)
            except ValueError:
                print('Data invalid')

        return value


if __name__ == '__main__':
    plot = Plot(xlim=xlim, ylim=ylim)
    stm = Port(device='/dev/ttyACM0', baudrate=9600)

    # start plotting
    control = plot.plot_live(auto_flush=True)
    next(control)
    control.send(stm.take_data(fresh=False))

    while True:
        value = stm.take_data(fresh=True)
        control.send(value)
        time.sleep(0.01)

    plot.root.mainloop()

