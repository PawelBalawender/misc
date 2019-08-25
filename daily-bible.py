#!/usr/bin/env python3
"""
This parses niedziela.pl website and spits out daily Bible fragments
"""
import arrow
import requests
from bs4 import BeautifulSoup

URL = 'https://niezbednik.niedziela.pl/liturgia/{}'


def get_url_for_today():
    utc = arrow.utcnow()
    local = utc.to('Europe/Warsaw')
    today = local.format('YYYY-MM-DD')
    url = URL.format(today)
    return url


url = get_url_for_today()
resp = requests.get(url)
html = resp.text
soup = BeautifulSoup(html, features='lxml')
div = soup.find('div', {'id': 'tabnowy0all'})

# now, split this div to a few sections separated by <h2>...</h2>
sections = []

# beware of '\n's as children of type NavigableString instead of Tag!
#x = [(i, type(i)) for i in div.children]
#for x in x: print(x)

for i in div.children:
    if i == '\n': continue
    if str(i).startswith('<h2>'):
        sections.append({'header': i.text, 'paragraphs': []})
    else:
        sections[-1]['paragraphs'] += i.text.splitlines()


readings = [{'header': sec['header'], 'text': ' '.join(sec['paragraphs'])} for sec in sections]


for i in readings:
    print(i)
    print('\n')
