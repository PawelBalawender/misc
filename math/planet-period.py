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
# to days
avg_periods = [i / (3600 * 24) for i in avg_periods]

# find avg duration of eclipses
full_times = [0, 0]
for i, ecl in enumerate(ecls):
    full_times[i] = sum((chunk[-1]-chunk[0]).total_seconds() for chunk in ecl)
avg_durations = [full_times[0] / len(ecls[0]), full_times[1] / len(ecls[1])]

# find avg brightness fluctuation
flux = []
flux.append([val for (dt, val) in data['Gwiazda 1'].items() if val < 0.985])
flux.append([val for (dt, val) in data['Gwiazda 3'].items() if val < 0.990])
avg_flux = sum(flux[0]) / len(flux[0]), sum(flux[1]) / len(flux[1])
avg_flux = [1-i for i in avg_flux]

# find planet sizes
star_radii = 772227, 150271  # km
planet_radii = [rad*(flx**0.5) for (rad, flx) in zip(star_radii, avg_flux)]

print('Planet radii in km:', *planet_radii)
print('Plant period in days:', *avg_periods)

