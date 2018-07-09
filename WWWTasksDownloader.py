#!/usr/bin/env python
# -*- coding: UTF-8
"""
This module downloads qualification tasks
from "WWW" workshops and saves them
"""
import os

import requests
from bs4 import BeautifulSoup

domain = 'https://warsztatywww.pl'

src = requests.get(domain + '/2018/program/').content
soup = BeautifulSoup(src, 'html.parser')

# get names of workshops
headers = []
for big in soup.find_all('big'):
    if big.a:
        continue
    else:
        headers += [big.text]

# get links to qualification tasks if they are, else append None
hrefs = []
for div in soup.find_all('div', {'class': 'panel panel-default'}):
    for td in div.find_all('td', {'class': 'col-sm-9'}):
        if td.a:
            hrefs += [domain + td.a['href']]
            break
    else:
        hrefs += [None]

# create dict {name - tasks} just for those workshops which have qual. tasks
sets = dict()
for name, link in zip(headers, hrefs):
    if link:
        sets[name] = link

base_dir = 'WWW tasks/'
if not os.path.isdir(base_dir):
    os.mkdir(base_dir)
sec_dir = '2018/'

if not os.path.isdir(sec_dir):
	os.mkdir(sec_dir)
base_dir += sec_dir

for title, url in sets.items():
    resp = requests.get(url, stream=True)

    title = title.replace('?', '').replace('/', '')

    file_name = base_dir + title + '.pdf'

    with open(file_name, 'wb') as doc:
        for chunk in resp.iter_content(4096):
            doc.write(chunk)
    del resp
