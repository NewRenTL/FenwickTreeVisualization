import matplotlib.pyplot as plt
import numpy as np
import timeit
from matplotlib.ticker import ScalarFormatter

class FenwickTree:
    def __init__(self, data):
        self.size = len(data) + 1
        self.tree = [0] * self.size
        for i in range(len(data)):
            self.update(i, data[i])

    def update(self, index, delta):
        index += 1
        while index < self.size:
            self.tree[index] += delta
            index += index & -index

    def prefix_sum(self, index):
        index += 1
        result = 0
        while index > 0:
            result += self.tree[index]
            index -= index & -index
        return result

class SegmentTree:
    def __init__(self, data):
        self.size = len(data)
        self.tree = [0] * (4 * self.size)
        self.data = data
        self.build_tree(1, 0, self.size - 1)

    def build_tree(self, v, tl, tr):
        if tl == tr:
            self.tree[v] = self.data[tl]
        else:
            tm = (tl + tr) // 2
            self.build_tree(2 * v, tl, tm)
            self.build_tree(2 * v + 1, tm + 1, tr)
            self.tree[v] = self.tree[2 * v] + self.tree[2 * v + 1]

    def query(self, v, tl, tr, l, r):
        if l > r:
            return 0
        if l == tl and r == tr:
            return self.tree[v]
        tm = (tl + tr) // 2
        return self.query(2 * v, tl, tm, l, min(r, tm)) + self.query(2 * v + 1, tm + 1, tr, max(l, tm + 1), r)

    def update(self, v, tl, tr, pos, new_val):
        if tl == tr:
            self.tree[v] = new_val
        else:
            tm = (tl + tr) // 2
            if pos <= tm:
                self.update(2 * v, tl, tm, pos, new_val)
            else:
                self.update(2 * v + 1, tm + 1, tr, pos, new_val)
            self.tree[v] = self.tree[2 * v] + self.tree[2 * v + 1]

def run_experiment(structure, data, num_queries):
    # Crear la estructura (Fenwick Tree o Segment Tree)
    if structure == "fenwick":
        data_structure = FenwickTree(data)
    elif structure == "segment":
        data_structure = SegmentTree(data)

    # Medir el tiempo para operaciones de suma acumulativa
    start_time = timeit.default_timer()
    for _ in range(num_queries):
        start_idx = np.random.randint(0, len(data) - 1)
        end_idx = np.random.randint(start_idx, len(data) - 1)
        if structure == "fenwick":
            result = data_structure.prefix_sum(end_idx) - data_structure.prefix_sum(start_idx - 1)
        elif structure == "segment":
            result = data_structure.query(1, 0, len(data) - 1, start_idx, end_idx)
    elapsed_time = timeit.default_timer() - start_time

    return elapsed_time

def plot_comparison(size, num_queries):
    sizes = [2**i for i in range(1, size + 1)]

    # Medir el tiempo para Fenwick Tree
    fenwick_times = [run_experiment("fenwick", np.random.randint(1, 10, s), num_queries) for s in sizes]

    # Medir el tiempo para Segment Tree
    segment_times = [run_experiment("segment", np.random.randint(1, 10, s), num_queries) for s in sizes]

    # Crear gráficos logarítmicos
    plt.plot(sizes, fenwick_times, label="Fenwick Tree")
    plt.plot(sizes, segment_times, label="Segment Tree")
    plt.xscale("log", base=2)
    plt.yscale("log")
    plt.xlabel("Size of Data Structure")
    plt.ylabel("Time (seconds)")
    plt.legend()
    plt.title("Comparison of Fenwick Tree and Segment Tree in Cumulative Sum Operations")
    plt.show()

# Definir el tamaño y el número de consultas
data_structure_size = 25  # Puedes ajustar este valor según tus necesidades
number_of_queries = 1000  # Puedes ajustar este valor según tus necesidades

# Generar y mostrar el gráfico de comparación
plot_comparison(data_structure_size, number_of_queries)
