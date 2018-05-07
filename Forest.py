#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
"""
It should be an implementation on a forest-graph.
But I didn't know what is forest.
And it sucks.

Best regards
Pawel
"""
import timeit
# todo: use NetworkX or NumPy for efficiency


class Forest:
    """
    Undirected graph with no cycles

    Stored in an adjacency matrix
    """
    def __init__(self):
        self.matrix = [[False]]

    def __getattr__(self, item):  # getattr is a fallback for getattribute func
        if item == 'edges':
            return self.get_edges()
        elif item == 'nodes':
            return self.get_nodes()
        raise AttributeError(f'An instance has no attribute \'{item}\'')

    def get_edges(self):
        occurrences = []
        for i, row in enumerate(self.matrix):
            occurrences += [[i for (i, x) in enumerate(row) if x]]

        edges = []
        for i, row in enumerate(occurrences):
            edges += [(i, j) for j in row]

        # formatting
        edges = [(min(i), max(i)) for i in edges]
        edges.sort()
        return edges

    def get_nodes(self):
        return set(i for i in range(len(self.matrix)))

    def add_edges(self, s: list) -> bool:
        if not s:
            return True

        temp_matrix = self.matrix

        # ensure there's no cycle
        max_node = len(temp_matrix) - 1
        for i in s:
            if i[0] <= max_node and i[1] <= max_node:
                return False
            max_node = max(max_node, *i)

        # make matrix big enough
        highest_node = max(i for j in s for i in j)
        size = highest_node + 1  # because of 0 index
        difference = size - len(temp_matrix)
        temp_matrix = [i + [False] * difference for i in temp_matrix]
        temp_matrix += [[False] * size for _ in range(difference)]

        for i in s:
            # add new edge
            temp_matrix[i[0]][i[1]] = True

        self.matrix = temp_matrix
        return True


def test_add_edges():
    a = Forest()
    edges = [(0, 1), (0, 2),
             (1, 3), (1, 4),
             (2, 5),
             (5, 6), (5, 7)]
    assert a.add_edges(edges)
    assert a.edges == edges
    assert a.nodes == set(i for j in edges for i in j)

    a = Forest()
    edges = [(0, 1), (0, 2),
             (1, 2)]
    assert a.add_edges(edges) is False
    assert not a.edges

    a = Forest()
    edges = [(0, 1), (0, 2),
             (1, 3), (1, 4), (1, 5),
             (2, 6), (2, 7),
             (8, 0), (2, 8)]

    assert a.add_edges(edges) is False
    assert not a.edges

    a = Forest()
    assert a.add_edges(edges[:7])
    assert a.add_edges(edges[7:]) is False
    assert a.edges == edges[:7]
    assert a.nodes == set(i for j in edges[:7] for i in j)

    a = Forest()
    edges = [(0, 1), (0, 2),
             (3, 1), (4, 1),
             (2, 5), (6, 2)]
    assert a.add_edges(edges)
    assert a.edges != edges  # different order in particular edges
    assert a.nodes == set(i for j in edges for i in j)

    a = Forest()
    edges = [(0, 1), (0, 2),
             (1, 3), (2, 3)]
    assert a.add_edges(edges) is False
    assert not a.edges


if __name__ == '__main__':
    test_add_edges()

    edge_number = 5
    # edges = ''.join(f'({i}, {i + 1}), ' for i in range(edge_number))
    # edges = '[' + edges[:-2] + ']'
    edges = [(i, i + 1) for i in range(edge_number)]

    for trials, number in zip([100,    500,    100,    10,     2,      1],
                              [1,       2,      10,     100,    500,    1000]):
        times = []
        for _ in range(trials):
            forest = Forest()
            times += [timeit.timeit('forest.add_edges(edges)',
                                    setup='from __main__ import forest, edges',
                                    number=number)]

        name_part = f'{trials}_trials_of_{number}_repeats.txt'
        file_name = './new_records/' + name_part
        with open(file_name, 'w+') as doc:
            doc.write(name_part + '\n')
            for i in times:
                doc.write(str(i) + '\n')

        print(name_part[:-4])
        print('min:', min(times))
        print('max:', max(times))
        print('avg:', sum(times) / len(times))

