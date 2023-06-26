import matplotlib.pyplot as plt
import numpy as np

with open("perf/parallel.txt", "r") as f:
    parallel = f.readlines()
    parallel = [float(x.strip()) for x in parallel]

with open("perf/sequential.txt", "r") as f:
    sequential = f.readlines()
    sequential = [float(x.strip()) for x in sequential]

print(np.mean(parallel), np.mean(sequential))

plt.hist(parallel, bins=40, alpha=0.45, range=(0.5,40.5), color='red')
plt.hist(sequential, bins=40, alpha=0.45, range=(0.5,40.5), color='blue')

plt.title("Computing time")

plt.legend(['Parallel',
            'Sequential'])

plt.show()
