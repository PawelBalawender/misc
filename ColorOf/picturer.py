import pathlib
from random import choices

from colorthief import ColorThief
from PIL import Image
import numpy as np
from numpy import f2py

with open('kmeans.f90', 'rb') as doc:
    f2py.compile(doc.read(), modulename='kmeans')

import sys
sys.exit()
DIR = r'C:\Users\Pawel\Desktop\arch\Życie\Zdjęcia\Zwykłe Zdjęcia\Eventy'
IMG_PATH = r'C:\Users\Pawel\Desktop\arch\Życie\Zdjęcia\Zwykłe Zdjęcia\Domowe\Fela\IMG_20171220_190732.jpg'
TEMP_IMG_PATH = r'__temp__.jpg'
TEMP_DOM_PATH = r'__data__.json'
DB_OK = r'__2__'
OUTNAME =  r'__OUTPUT__.jpg'
width = 15
height = 15

# todo: normalize filenames
# todo: what quality is best
# todo: updating the db, not writing from the beginning each time
# todo: make it faster
# todo: fix data from save_dominants


def save_shrunk():
    img = Image.open(IMG_PATH)
    img = img.resize((width, height))
    img.save(TEMP_IMG_PATH)


def get_pixels():
    img = Image.open(TEMP_IMG_PATH)
    pixels = np.asarray(img)
    return pixels


def save_dominants():
    def dominants():
        folder = pathlib.Path(DIR)
        lim = width * height
        for img_path in folder.glob('**/*.jpg'):
            if not lim:
                raise StopIteration
            lim -= 1
            print(lim)
            color_thief = ColorThief(img_path)
            dominant = color_thief.get_color(quality=715)
            yield img_path, dominant

    gen = dominants()
    next(gen)

    with open(TEMP_DOM_PATH, 'w') as doc:
        for path, colour in gen:
            doc.write(f'{str(path)};{str(colour)}\n')


def refactor_db():
    l = len('"WindowsPath(\'')
    ending_seq = '\')";('

    def change_line(line):
        end = line.index(ending_seq)
        return line[l:end] + ';' + line[end+len(ending_seq):-2] + '\n'

    with open(TEMP_DOM_PATH, 'r') as doc_r, open(DB_OK, 'w') as doc_w:
        lines = doc_r.readlines()
        lines = map(change_line, lines)
        doc_w.writelines(lines)


def get_ordered_px_db():
    with open(DB_OK) as doc:
        data = doc.readlines()
    data = [i.split(';') for i in data]
    data = [[list(map(int, i[1][:-1].split(', '))), i[0]] for i in data]
    data = data[:width*height]
    data.sort(key=lambda x: x[0])
    return data


def get_ordered_px_photo():
    # fixme: use reshape
    wanted = get_pixels()
    wanted = [record for row in wanted for record in row]
    wanted = [(record, count) for (count, record) in enumerate(wanted)]
    wanted.sort(key=lambda x: tuple(x[0].data))
    return wanted


def get_new_matrix():
    db = get_ordered_px_db()
    photo = get_ordered_px_photo()

    new = [0 for i in range(width*height)]
    for db_p, photo_p in zip(db, photo):
        new[photo_p[1]] = db_p[0]

    new = np.reshape(new, (width, height, 3))
    return new.astype('uint8')


def save_new_photo():
    mx = get_new_matrix()
    img = Image.fromarray(mx)
    img.save(OUTNAME)


def k_means():
    pixels = get_pixels()
    pixels = pixels.reshape(width*height, 3)
    pixels = [[(pos // width, pos % width), px] for (pos, px) in enumerate(pixels)]
    centroids = [[pos[0][0], pos[0][1], []] for pos in choices(pixels, k=10)]
    end_cond = False
    last_cent = -1,-1

    while not end_cond:
        # find closest centroids for each px
        for px in pixels:
            d = max(width, height) + 1
            cent = centroids[0]
            cent[2].append(px)

            for temp_cent in centroids:
                temp_d = (px[0][0]-temp_cent[0])**2 + (px[0][1]-temp_cent[1])**2
                if temp_d < d:
                    # what we use there is that (cent IS centroids[n])
                    cent[2].pop()
                    cent = temp_cent
                    cent[2].append(px)
        last_cents = [(i[0], i[1]) for i in centroids]
        # move centroids == cluster centers
        for i in centroids:
            i[0] = sum(px[0][0] for px in i[2]) / (len(i[2]) or 1)
            i[1] = sum(px[0][1] for px in i[2]) / (len(i[2]) or 1)

        a = all(abs(centroids[i][0] - last_cents[i][0]) < 1 for i in range(len(centroids)))
        b = all(abs(centroids[i][1] - last_cents[i][1]) < 1 for i in range(len(centroids)))
        if a and b:
            break

    return centroids


def get_dominant():
    centroids = k_means()
    for i in centroids:
        print(i[0], i[1])


get_dominant()

