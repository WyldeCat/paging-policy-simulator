from matplotlib import pyplot as plt
import numpy as np

def write_graph(csv, out, title):
    array = np.loadtxt(csv, delimiter=",", dtype=int)
    array = array.transpose()

    intervals = array[0].astype(float) / 1000

    plt.plot(intervals, array[1], label='access')
    plt.plot(intervals, array[2], label='miss')
    plt.plot(intervals, array[3], label='eviction')
    plt.title(title)
    plt.xlabel('timeline(ms)')
    plt.legend()
    plt.savefig(out)
