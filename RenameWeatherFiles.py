import glob
import os


if __name__ == '__main__':
    if input('Modify D/weather directory? [Y/n]').lower() in {'', 'n'}:
        assert 0

    for filename in glob.glob('D:\weather\\*'):
        base, name = os.path.split(filename)
        name, ext = os.path.splitext(name)
        new_name = name[:15] + ext
        new = os.path.join(base, new_name)

        os.rename(filename, new)

