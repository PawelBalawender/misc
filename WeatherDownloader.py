"""
AKS Precision Ball Poland Sp. o.o., Przemysłowa, Żarów
50°56'50.8"N 16°30'30.0"E
50.947456, 16.508344
"""
import datetime
import calendar
import json
import sys
import os

from bs4 import BeautifulSoup
import requests

LAT = 50.947456
LON = 16.508344
# It should be valid until 10.10.2018
API_KEY = 'e9906b44c0ef49fe80f161914181009'

BASE_URL = 'https://api.worldweatheronline.com/premium/v1/past-weather.ashx'

file_name = ''


def download(year: int, month: int, save: bool) -> bytearray:
    global file_name

    today = datetime.date.today()
    if (today.year, today.month) == (year, month):
        last_day = today.day
    else:
        last_day = calendar.monthrange(year, month)[1]
    date_start = '{}-{:02}-01'.format(year, month)
    date_end = '{}-{:02}-{}'.format(year, month, last_day)

    payload = {
        'q': '{},{}'.format(LAT, LON),
        'extra': 'utcDateTime',
        'date': date_start,
        'enddate': date_end,
        'includelocation': 'yes',
        'tp': '1',
        'key': API_KEY,
    }

    resp = requests.get(BASE_URL, params=payload)

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
    fn = os.path.splitext(file_name)[0] + '.json'
    with open(fn, 'w') as doc:
        json.dump(_fields, doc)


def months_range(y1, m1, y2, m2):
    for year in range(y1, y2+1):
        for month in range(1, 12+1):
            if year == y1 and month < m1:
                continue
            if year == y2 and month > m2:
                break
            yield (year, month)


if __name__ == '__main__':
    if input('Gonna make a few requests to the weather api. Proceed? [Y/n]').lower() == 'n':
        sys.exit()

    start = (2008, 6)
    end = (2018, 6)

    for i in months_range(*start, *end):
        print('downloading and saving:', i[0], i[1])
        xml = download(*i, True)
        fields = extract(xml)
        save_extracted(fields)
