#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
This module plots CSV files containing the data collected by
a ventilation central.

Features:
    -choosable language: pl/eng
    -you are able to hide plot's line by clicking on its legend
        (credit: @stackoverflow Joe Kington
        https://stackoverflow.com/questions/31410043/hiding-lines-after-showing-a-pyplot-figure/31417070#31417070)
    -error handling
    -logging progress to stdout

CSV format:
DD;MM;YYYY;HH;MM;t0;t1;t2;t3;t4;t5;t6;p0;p1;p2;b0;b1;b2

Where:
    DD MM YYYY HH MM: datetime
    t[0:6]: 7 columns of temperature values from the ventilation central
    p[0:2]: 3 parameters of the ventilation
    b[0:2]: 3 binary values corresponding to central's configuration

Usage:
    Pass some directory's path to DEFAULT_PATH to control where will
        File Explorer open for file dialog
    Pass some file's path to FILE variable to set the file to analyze and plot
        File dialog won't be open if it's set and the file exists
    Modifying LABELS would have the same effect as modifying headers of columns
        in the CSV data source file - each field of LABELS tuple corresponds to
        one curve on the plot, in the same order
    Set LOCAL_LANG to True to enable translating logs and text on the plot to
        the local language (this local language is determined by locale module
        based on system environment variables), if only translations to this
        language are present in the code
"""
import tkinter as tk
from tkinter import filedialog
import datetime
import pathlib
import locale
import json
import glob
import csv
import sys

DEFAULT_PATH = "C:/Users/Pawel/Desktop"
DEFAULT_PATH = pathlib.Path(DEFAULT_PATH)
FILE = "DaneTaty.csv"
FILE = DEFAULT_PATH.joinpath(FILE)
WEATHER_DATA_PATH = "D:/weather"
WEATHER_FILENAME_FORMAT = "weather_{y}-{m:02}.json"

LABELS = ('Outdoor temperature',
          'Supply temperature',
          'Exhaust temperature',
          'Temperature on thermometer 1',
          'Temperature on thermometer 2',
          'Temperature on thermometer 3',
          'Temperature on thermometer 4')
LOCAL_LANG = False

if LOCAL_LANG:
    locale.setlocale(locale.LC_ALL, '')
    if locale.getlocale() == ('Polish_Poland', '1250'):
        LANG = 'pl_PL'
    else:
        # lang unknown, so use default
        LANG = 'en_US'
else:
    LANG = 'en_US'

translations = {
    'No Matplotlib library found on this computer. Install it to use this script': 'Biblioteka Matplotlib nie została odnaleziona na tym komputerze. Zainstaluj ją, by móc używać tego skryptu',
    'Script terminated by user. Exitting...': 'Program przerwany przez użytkownika. Zamykanie programu...',
    'The file is invalid. Exiting...': 'Plik zawiera nieprawidłowe znaki. Zamykanie programu...',
    'CSV data from file: {} read successfully': 'Pomyślnie wczytano dane CSV z pliku: {}',
    'The file data doesn\'t match the configured format. Exitting...': 'Dane w pliku nie odpowiadają skonfigurowanemu formatowi. Kończenie pracy programu...',
    'Timestamps extracted': 'Pomyślnie wczytano dane o czasie',
    'Temperature values extracted': 'Pomyślnie wczytano wartości temperatur',
    'Temperature [{}C]': 'Temperatura [{}C]',
    'Date and time': 'Data i godzina',
    'Plot initialized': 'Wykres zainicjalizowany',
    'Plotting: {}': 'Rysowanie wykresu dla: {}',
    'Outdoor temperature': 'Temperatura zewnętrzna',
    'Supply temperature': 'Temperatura nawiewu',
    'Exhaust temperature': 'Temperatura wywiewu',
    'Temperature on thermometer 1': 'Temperatura na termometrze 1',
    'Temperature on thermometer 2': 'Temperatura na termometrze 2',
    'Temperature on thermometer 3': 'Temperatura na termometrze 3',
    'Temperature on thermometer 4': 'Temperatura na termometrze 4',
    'Language not supported': 'Language not supported',  # it's an exceptional case
    'Real temperatures': 'Realne temperatury',
}


class InteractiveLegend(object):
    def __init__(self, legend):
        self.legend = legend
        self.fig = legend.axes.figure

        self.lookup_artist, self.lookup_handle = self._build_lookups(legend)
        self._setup_connections()

        self.update()

    def _setup_connections(self):
        for artist in self.legend.texts + self.legend.legendHandles:
            artist.set_picker(10)  # 10 points tolerance

        self.fig.canvas.mpl_connect('pick_event', self.on_pick)
        self.fig.canvas.mpl_connect('button_press_event', self.on_click)

    @staticmethod
    def _build_lookups(legend):
        labels = [t.get_text() for t in legend.texts]
        handles = legend.legendHandles
        label2handle = dict(zip(labels, handles))
        handle2text = dict(zip(handles, legend.texts))

        lookup_artist = {}
        lookup_handle = {}
        for artist in legend.axes.get_children():
            if artist.get_label() in labels:
                handle = label2handle[artist.get_label()]
                lookup_handle[artist] = handle
                lookup_artist[handle] = artist
                lookup_artist[handle2text[handle]] = artist

        lookup_handle.update(zip(handles, handles))
        lookup_handle.update(zip(legend.texts, handles))

        return lookup_artist, lookup_handle

    def on_pick(self, event):
        handle = event.artist
        if handle in self.lookup_artist:
            artist = self.lookup_artist[handle]
            artist.set_visible(not artist.get_visible())
            self.update()

    def on_click(self, event):
        if event.button == 3:
            visible = False
        elif event.button == 2:
            visible = True
        else:
            return

        for artist in self.lookup_artist.values():
            artist.set_visible(visible)
        self.update()

    def update(self):
        for artist in self.lookup_artist.values():
            handle = self.lookup_handle[artist]
            if artist.get_visible():
                handle.set_visible(True)
            else:
                handle.set_visible(False)
        self.fig.canvas.draw()

    @staticmethod
    def show():
        plt.show()


def _(msg: str) -> str:
    if LANG == 'en_US':
        return msg
    elif LANG == 'pl_PL':
        return translations[msg]
    else:
        print(_('Language not supported.'))
        sys.exit()


def months_range(y1, m1, y2, m2):
    for year in range(y1, y2+1):
        for month in range(1, 12+1):
            if year == y1 and month < m1:
                continue
            if year == y2 and month > m2:
                break
            yield (year, month)


def get_temperature_data(since: datetime.date, till: datetime.date):
    def to_datetime(date, hour):
        string = '{} {}'.format(date, hour.zfill(4))
        return datetime.datetime.strptime(string, '%Y-%m-%d %H%M')
    # fixme: it cuts 4 hours at the last month's day
    first_day = since.day
    last_day = till.day
    first_month = since.year, since.month
    last_month = till.year, till.month

    base = pathlib.Path(WEATHER_DATA_PATH)

    logs = []
    r = list(months_range(*first_month, *last_month))
    for year, month in r:
        filename = WEATHER_FILENAME_FORMAT.format(y=year, m=month)
        filename = base.joinpath(filename).absolute()
        file = glob.glob(str(filename))[0]
        with open(file) as doc:
            logs += json.load(doc)
    logs = ((to_datetime(i[0], i[1]), int(i[2])) for i in logs)

    # now trim the logs we needn't\
    new_logs = []
    for i in logs:
        if i[0].year == first_month[0]:
            if i[0].month < first_month[1]:
                continue
            elif i[0].month == first_month[1] and i[0].day < first_day:
                continue

        if i[0].year == last_month[0]:
            if i[0].month > last_month[1]:
                break
            elif i[0].month == last_month[1] and i[0].day > last_day:
                break
        new_logs.append(i)
    return new_logs


try:
    import matplotlib.pyplot as plt
    import matplotlib.dates as dates
except ImportError:
    print(_('No Matplotlib library found on this computer. Install it to use this script'))
    sys.exit()

if not pathlib.Path.is_file(FILE):
    root = tk.Tk()
    root.withdraw()
    while not pathlib.Path.is_file(FILE):
        response = filedialog.askopenfilename()
        if not response:
            print(_('Script terminated by user. Exitting...'))
            sys.exit()
        FILE = pathlib.Path(response)
    root.destroy()

# read file to memory
with open(FILE) as doc:
    reader = csv.reader(doc, delimiter=';')
    try:
        data = [row for row in reader]
    except UnicodeDecodeError:
        print(_('The file is invalid. Exiting...'))
        sys.exit()
print(_('CSV data from file: {} read successfully').format(FILE))

# get datetimes
fmt = '%d %m %Y %H %M'
strptime = datetime.datetime.strptime
date_strings = (' '.join(i[:5]) for i in data)
try:
    timestamps = list(map(lambda string: strptime(string, fmt), date_strings))
except ValueError:
    print(_('The file data doesn\'t match the configured format. Exitting...'))
    sys.exit()
print(_('Timestamps extracted'))

# get temperatures
temperature_sets = [i[5:12] for i in data]
temperature_sets = zip(*temperature_sets)  # transpose
temperature_sets = (map(int, temps) for temps in temperature_sets)
temperature_sets = (map(lambda x: x / 10, temps) for temps in temperature_sets)
print(_('Temperature values extracted'))

# convert to list for data analysis and matplotlib
temperature_sets = [list(i) for i in temperature_sets]

# initialize plot
fig = plt.figure(figsize=(16, 9))
ax = plt.subplot(111)

# make up the plot
minimals, maximals = (min(temps) for temps in temperature_sets), (max(temps) for temps in temperature_sets)
min_temp, max_temp = min(minimals), max(maximals)
min_temp, max_temp = int(min_temp * 10), int(max_temp * 10)  # scale it for range()
ticks = [float(i / 10) for i in range(min_temp, max_temp, 10)]
ax.set_yticks(ticks)

ax.xaxis.set_minor_locator(dates.HourLocator(interval=6))
ax.xaxis.set_minor_formatter(dates.DateFormatter('%H:%M'))
ax.xaxis.set_major_locator(dates.DayLocator())
ax.xaxis.set_major_formatter(dates.DateFormatter('%B %Y %d'))

ax.xaxis.set_tick_params(which='minor', rotation=45)
ax.xaxis.set_tick_params(which='major', pad=35, rotation=45)\

# \u00b0 - degree symbol
ax.set_ylabel(_('Temperature [{}C]').format('\u00b0'))
ax.set_xlabel(_('Date and time'))
print(_('Plot initialized'))

for i, temps in enumerate(temperature_sets):
    print(_('Plotting: {}').format(_(LABELS[i])))
    ax.plot_date(timestamps, temps, '-', label=_(LABELS[i]))

# now plot real outside temperatures
real_temps = [temp for (_, temp) in get_temperature_data(timestamps[0].date(), timestamps[-1].date())]
stamps = [timestamps[0] + datetime.timedelta(hours=i) for i in range(len(real_temps))]
ax.plot_date(stamps, real_temps, '-', label=_('Real temperature'))

# plt.subplots_adjust(top=0.9, bottom=0.2)
fig.tight_layout()

leg = ax.legend()
interactive_legend = InteractiveLegend(leg)
interactive_legend.show()
