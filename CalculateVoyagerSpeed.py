import arrow


vgr2 = {
        #'start_date': '19 nov 2018 1:48 AM, CANB',
        #'start_date': '19 nov 2018 1:48 AM, +1100',
        #'start_range': 17.89e9,
        'start_date': '20 aug 1977 2:29 PM, +0000',
        'start_range': 0,
        #'end_date': '12 jan 2019 8:31 PM, CANB',
        'end_date': '11 jan 2019 8:31 PM, +1100',
        'end_range': 18.01e9,
        }

mro = {
        #'start_date': '18 nov 2018 8:03 PM, MADR',
        'start_date': '18 nov 2018 8:03 PM, +0100',
        'start_range': 136.65e6,
        #'end_date': '12 jan 2019 12:54 AM, MADR',
        'end_date': '12 jan 2019 12:54 AM, +0100',
        'end_range': 202.13e6,
        }

soho = {
        #'start_date': '18 nov 2018 6:51 AM, GOLD',
        'start_date': '18 nov 2018 6:51 AM, -0800',
        'start_range': 1.66e6,
        #'end_date': '12 jan 2019 10:32 AM, MADR',
        'end_date': '12 jan 2019 10:32 AM, +0100',
        'end_range': 1.36e6,
        }

date_fmt = 'DD MMM YYYY H:mm A, Z'
for data, name in zip([vgr2, mro, soho], ['VGR2', 'MRO', 'SOHO']):
    range_delta = data['end_range'] - data['start_range']

    end_date = arrow.get(data['end_date'], date_fmt)
    start_date = arrow.get(data['start_date'], date_fmt)
    time_delta = (end_date - start_date).total_seconds()

    msg = '{}: {} km in {} s = {} km/s'
    print(msg.format(name, range_delta, time_delta, range_delta/time_delta))

