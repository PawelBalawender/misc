#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
from typing import Tuple
import tkinter
import serial
import struct
import time

# ADC resolution in bits
DATA_RES = 16

xlim = 2 ** DATA_RES - 1
ylim = 2 ** DATA_RES - 1

Point = Tuple[int, int]


class Plot:
    def __init__(self, xlim: int=1, ylim: int=1, device=None):
        self.device=device
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

        self.canvas.bind('<Button-1>', self.on_click)
        self.canvas.pack(expand=tkinter.YES, fill=tkinter.BOTH)
        #self.root.mainloop()

        #assert abs(self.unscale_x(self.scale_x(120)) - 120) < self.ax_xsize
        #assert abs(self.scale_x(self.unscale_x(1200)) - 1200) < 1

    def on_click(self, event):
        x, y = self.unscale_x(event.x), self.unscale_y(event.y)
        print('x,y:', x, y)
        w = self.device.port.write
        a = (x << 16 | y)
        print(hex(a))
        # 'I': unsigned int, big endian (reverse to receive it in correct order on stm)
        w(struct.pack('>I', a))
        w(b'\xFF\x0F')

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

    def unscale_x(self, x: int) -> int:
        # position x's coordinate of pixel on the plot to "real-world" value
        return int((x - self.ox - 1) / self.ax_xsize * self.xlim)

    def unscale_y(self, y: int) -> int:
        #position y's coordinate of pixel on the plot to "real-world" values
        return int((y - self.oy + 1) / -self.ax_ysize * self.ylim)

    def scale_x(self, x: float) -> int:
        # position some arbitrary plot's x value to a real x coord in the window
        return int(self.ox+1 + (x / self.xlim) * self.ax_xsize)

    def scale_y(self, y: int) -> None:
        return int(self.oy-1 - (y / self.ylim) * self.ax_ysize)

    def plot(self, a: Point, b: Point) -> None:
        a = self.scale_x(a[0]), self.scale_y(a[1])
        b = self.scale_x(b[0]), self.scale_y(b[1])
        self.canvas.create_line(a[0], a[1], b[0], b[1], fill='red')

    def plot_car(self, x: int, y: int, c: str) -> None:
        x, y = self.scale_x(x), self.scale_y(y)
        self.canvas.create_rectangle(x-10, y-10, x+10, y+10, fill=c,
                outline=c)

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

    def car_plotter(self):
        data = xlim/2, ylim/2

        while data not in {self.restart, self.stop}:
            # rm last car
            self.plot_car(data[0], data[1], self.background_color)
            data = yield

            self.plot_car(data[0], data[1], 'red')
            self.canvas.update()



class Port:
    def __init__(self, device: str, baudrate: int):
        self.port = serial.Serial('/dev/ttyACM0', baudrate=9600,
                bytesize=8)
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

    def take_car_pos(self, fresh: bool=False) -> Point:
        if fresh:
            self.port.reset_input_buffer()
            self.port.readline()
            self.port.read()

        value = None
        while value is None:
            temp = self.port.readline()[:-2]
            #print(temp)
            self.port.read()
            if len(temp) == 6 and temp[0] == b'\x00': temp = temp[1:]
            if len(temp) == 5 and temp[-1] == b'\x00': temp = temp[:-1]
            if len(temp) < 4: continue
            try: value = int.from_bytes(temp, byteorder='little', signed=False)
            except ValueError:
                print('Data invalid')
            return value >> 16, value & (2**16-1)


if __name__ == '__main__':
    stm = Port(device='/dev/ttyACM0', baudrate=9600)
    plot = Plot(xlim=xlim, ylim=ylim, device=stm)

    # start plotting
    #control = plot.plot_live(auto_flush=True)
    #next(control)
    #control.send(stm.take_data(fresh=False))
    plt = plot.car_plotter()
    next(plt)

    while True:
        #value = stm.take_data(fresh=True)
        values = stm.take_car_pos(fresh=True)
        plt.send(values)
        #control.send(value)
        time.sleep(0.01)

    plot.root.mainloop()

