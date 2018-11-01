n, t = list(map(int, input().split()))
ds = list(map(int, input().split()))
dest_x, dest_y = list(map(int, input().split()))

# jak ilosc inputu nie jest wielokortotnoscia 4, to powiel pare razy
# input tak, zeby ulosc inputu byla wielokrotnoscia 4
# nastepnie dopasuj t, tak, zeby ilosc cykli sie zgadzala
# nastepnie, po wszystkich, przejedz po ruchach ktorre zostaly

# znajdywanie czasu: sum(ds) + len(ds)

# dx: dest_x, dy: dest_y, ox: out_x, oy: out_y
def get_locations(steps):
    x = 0
    y = 0
    tt = 0 # time counter
    yield x,y,tt
    for i in range(len(steps)):
        for j in range(1, steps[i]+1):
            if i%4==0:y+=1
            if i%4==1:x+=1
            if i%4==2:y-=1
            if i%4==3:x-=1
            tt += 1
            yield x,y, tt
        tt += 1

locs = [i for i in get_locations(ds)]

out_x = locs[-1][0] - locs[0][0]
out_y = locs[-1][1] - locs[0][1]

# +1 cause it rotates right after the last move
full_time = locs[-1][2] + 1

#print(locs, full_time)


def get_possible(locations, full_t):
    for x, y, tt in locations:
        a = dest_x - x
        b = dest_y - y

        if out_x:
            # cycle x
            cx = a / out_x
        else:
            cx = 0

        if out_y:
            cy = b / out_y
        else:
            cy = 0

        if cx == cy == int(cx):
            z = int(cx) * full_time + tt
            if z < t:
                yield z

zz = [i for i in get_possible(locs, full_time)]
print(zz)
print(len(zz))
