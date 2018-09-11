"""
AKS Precision Ball Poland Sp. o.o., Przemysłowa, Żarów
50°56'50.8"N 16°30'30.0"E
50.947456, 16.508344
"""
import pathlib
import datetime
import calendar
import json

import matplotlib.pyplot as plt
from bs4 import BeautifulSoup
import requests

fig = plt.figure()
ax = fig.add_subplot(111)

file_name = ''

def download(year: int, month: int, save: bool) -> bytearray:
    global file_name
    api_key = 'e9906b44c0ef49fe80f161914181009'
    base_url = 'https://api.worldweatheronline.com/premium/v1/past-weather.ashx'

    today = datetime.date.today()
    if (today.year, today.month) == (year, month):
        last_day = today.day
    else:
        last_day = calendar.monthrange(year, month)[1]
    date_start = '{}-{:02}-01'.format(year, month)
    date_end = '{}-{:02}-{}'.format(year, month, last_day)

    payload = {
        'q': '50.947456,16.508344',
        'extra': 'utcDateTime',
        'date': date_start,
        'enddate': date_end,
        'includelocation': 'yes',
        'tp': '1',
        'key': api_key,
    }

    resp = requests.get(base_url, params=payload)

    if save:
        file_name = 'weather_{}_{}.xml'.format(date_start, date_end)
        with open(file_name, 'wb') as doc:
            doc.write(resp.content)

    return resp.content


def extract(_xml: bytearray) -> list:
    # xml structure:
    # https://developer.worldweatheronline.com/api/docs/historical-weather-api.aspx
    soup = BeautifulSoup(_xml, 'lxml')
    days = soup.find_all('weather')

    days_fields = []
    for day in days:
        for hour in day.find_all('hourly'):
            date, time, temp = hour.utcdate, hour.utctime, hour.tempc
            date, time, temp = date.get_text(), time.get_text(), temp.get_text()
            days_fields.append((date, time, temp))

    return days_fields


def load_xml(file_name: str) -> str:
    with open(file_name) as doc:
        return doc.read()


def save_extracted(_fields):
    fn = pathlib.Path(file_name)
    fn = fn.with_suffix('.json')
    with open(fn, 'w') as doc:
        json.dump(_fields, doc)


xml = download(2018, 9, save=True)
# xml = load_xml('weather.xml')
fields = extract(xml)
save_extracted(fields)
