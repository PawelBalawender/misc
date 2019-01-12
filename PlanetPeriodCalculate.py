import csv

import arrow

# setup
data = {
        'Gwiazda 1': {},
        'Gwiazda 3': {},
        }

# read and parse data
with open('/home/pi/Downloads/obserwacje-1547291402.csv') as doc:
    reader = csv.reader(doc, delimiter=',')
    next(reader)
    for row in reader:
        if row[0] == 'Gwiazda 2': continue
        date = arrow.get(row[1], 'YYYY-MM-DD HH:mm')
        data[row[0]][date] = float(row[2])

# extract eclipse moments
dates = []
dates.append([dt for (dt, val) in data['Gwiazda 1'].items() if val < 0.985])
dates.append([dt for (dt, val) in data['Gwiazda 3'].items() if val < 0.990])
dates[0].sort()
dates[1].sort()

# divide eclipse moments to chunks by time (to separate each individual ecl.)
ecls = [[], []]
for i in range(2):
    ecls[i] = [
            [dates[i][0]],
            ]
    for dt in dates[i][1:]:
        if (dt - ecls[i][-1][-1]).total_seconds() > 15 * 60:
            ecls[i] += [[]]
        ecls[i][-1] += [dt]

# test against invalid chunking
for ecl in ecls:
    for chunk in ecl:
        last = chunk[0]
        for dt in chunk[1:]:
            assert (dt - last).total_seconds() == 15 * 60
            last = dt

# find avg periods
periods = [[], []]
for i, ecl in enumerate(ecls):
    last = ecl[0][-1]
    for chunk in ecl[1:]:
        periods[i] += [(chunk[0] - last).total_seconds()]
        last = chunk[-1]

avg_periods = []
avg_periods = [
        sum(periods[0]) / len(periods[0]),
        sum(periods[1]) / len(periods[1]),
        ]

# find avg duration of eclipses
full_times = [0, 0]
for i, ecl in enumerate(ecls):
    full_times[i] = sum((chunk[-1]-chunk[0]).total_seconds() for chunk in ecl)
avg_durations = [full_times[0] / len(ecls[0]), full_times[1] / len(ecls[1])]

print(avg_durations)
