from typing import Generator, Tuple
import math
import matplotlib.pyplot as plt

Point = Tuple[float, float]


def plotter_gen() -> Generator[None, Point, None]:
    fig = plt.figure()
    ax = fig.add_subplot(111)
    last_x, last_y = yield

    while True:
        x, y = yield
        ax.plot((last_x, x), (last_y, y), 'r-')
        last_x, last_y = x, y
        plt.pause(0.1)


def norm_angle(angle: float) -> float:
    angle = angle if angle > 0 else angle + math.tau
    angle %= math.tau
    return angle


def test_norm_angle():
    cases = [
        (0, 0),
        (math.pi, math.pi),
        (-math.pi/4, math.tau*7/8),
        (-math.pi/2, math.tau*6/8)
    ]
    angles, oks = zip(*cases)
    for angle, ok in zip(angles, oks):
        res = norm_angle(angle)
        assert abs(res - ok) < 0.001


def get_rotation(pos: Point, dest: Point, dir: float) -> float:
    return math.atan2(dest[1]-pos[1], dest[0]-pos[0]) - dir


def test_get_rotation():
    cases = [
        [(0, 0), (1, 1), 0.0, math.pi/4],
        [(0, 0), (-1, 1), 0.0, math.pi*3/4],
        [(0, 0), (-1, -1), math.pi, math.pi/4],
        [(0, 0), (1, -1), math.pi*3/4, math.pi],
    ]
    positions, dests, orients, oks = zip(*cases)
    for pos, dest, orient, ok in zip(positions, dests, orients, oks):
        res = get_rotation(pos, dest, orient)
        res = norm_angle(res)
        assert abs(res - ok) < 0.001


if __name__ == '__main__':
    test_norm_angle()
    test_get_rotation()
    plotter = plotter_gen()
    next(plotter)

    v = 1.0
    ang_v = 0.0
    x = y = 0.0
    orient = 0.0
    dest_x, dest_y = 3, 5

    plotter.send((x, y))
    for i in range(100):
        to_rotate = get_rotation((x, y), (dest_x, dest_y), orient)
        to_rotate = norm_angle(to_rotate)
        turn = max(min(to_rotate, 0.6), -0.6)

        orient += turn
        x += math.cos(orient) * v
        y += math.sin(orient) * v
        plotter.send((x, y))
