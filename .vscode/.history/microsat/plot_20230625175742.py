import matplotlib.pyplot as plt
import numpy as np

with open("perf/parallel.txt", "r") as f:
    parallel = f.readlines()
    parallel = [float(x.strip()) for x in parallel]

with open("perf/serial.txt", "r") as f:
    serial = f.readlines()
    serial = [float(x.strip()) for x in serial]

print(np.mean(parallel), np.mean(serial))

plt.hist(parallel, bins=40, alpha=0.45, range=(0.5,40.5), color='red')
plt.hist(serial, bins=40, alpha=0.45, range=(0.5,40.5), color='blue')

plt.title("Computing time")

plt.legend(['Parallel',
            'serial'])

plt.show()
